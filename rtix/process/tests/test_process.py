# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import time
import logging
from google.protobuf.wrappers_pb2 import Int64Value, BoolValue

from rtix.types import common_pb2
from rtix.ipc.node import Node
from rtix.core.status import Status
from rtix.process.process import Process, ProcessConfig, InputConfig, OutputConfig

from rtix.ipc.tests.test_channel_map import load_test_channel_map
from rtix.ipc.tests.test_node import ThreadWithAssertion

PROCESS_TEST_YAML_FILE = "/rtix/rtix/process/tests/channel-map.yaml"
MAX_PROCESS_TIME_S = 3.0
ACTION_VALUE = True
INPUT_VALUE = 567


class TestProcess(Process):
    __test__ = False

    def __init__(self, node, config, action):
        super().__init__(node, config, action)
        self.input = Int64Value()
        logging.info("Created test process")

    def handleAction(self, action: BoolValue):
        # Need to sleep to give the fixture node time to receive
        time.sleep(0.2)
        self.publishStatus(status=Status.SUCCESS, msg="Handled")
        self.setRunning(True)
        logging.info("Handled action")

    def stepOuter(self):
        if self.receiveInput(key="in", msg=self.input):
            logging.info("Received input")

        if self.getProcessTimeS() > MAX_PROCESS_TIME_S:
            self.stop()

    def stepInner(self):
        if self.input is not None:
            self.publishOutput(key="out", msg=self.input)
            logging.info(f"Published output {self.input.value}")


def test_process():
    # Assertions within a thread won't automatically fail the pytest test
    # because they are raised in a different context. To correctly capture and
    # propagate these failures, a mechanism is needed to re-raise exceptions in
    # the main thread.  We subclass Thread to raise an exception on join, and
    # pass the thread callbacks in the main loop below.

    channel_map = load_test_channel_map(yaml_file=PROCESS_TEST_YAML_FILE)

    def test_process_callback(channel_map):
        assert "test_node" in channel_map.nodes
        test_node = Node(config=channel_map.nodes["test_node"])

        # Create the process
        test_process = TestProcess(
            node=test_node,
            config=ProcessConfig(
                monitor_rate_s=0.1,
                loop_rate_s=0.05,
                action_channel_id="action",
                status_channel_id="status",
                input_channels={"in": InputConfig(channel_id="x")},
                output_channels={"out": OutputConfig(channel_id="y")},
            ),
            action=BoolValue(),
        )

        # Run for finite amount of time
        test_process.run()

    # Start the thread
    test_process_thread = ThreadWithAssertion(
        target=test_process_callback,
        channel_map=channel_map,
    )

    test_process_thread.start()

    # Access the fixture node after the test process has started
    assert "fixture_node" in channel_map.nodes
    fixture_node = Node(config=channel_map.nodes["fixture_node"])

    # Give the thread time to spin up
    time.sleep(0.2)

    # Send the action
    fixture_node.publisher(channel_id="action").send(msg=BoolValue(
        value=ACTION_VALUE))

    # Await status
    outcome = common_pb2.Outcome()
    fixture_node.subscriber(channel_id="status").recv(msg=outcome)

    # Send the 'x' channel
    fixture_node.publisher(channel_id="x").send(msg=Int64Value(
        value=INPUT_VALUE))

    # Give the process time to return the input value
    time.sleep(0.2)

    # Receive the 'y' channel
    output = Int64Value()
    fixture_node.subscriber(channel_id="y").recv(msg=output)

    test_process_thread.join()

    # These will only be true if the input and action was received
    assert outcome.status == common_pb2.Status.SUCCESS
    assert output.value == INPUT_VALUE


if __name__ == "__main__":
    test_process()
