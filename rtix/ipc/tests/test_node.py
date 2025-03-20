# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import threading
import time
from google.protobuf.wrappers_pb2 import Int64Value

from rtix.ipc.node import Node
from rtix.ipc.channel_map import ChannelMap
from rtix.ipc.tests.test_channel_map import load_test_channel_map


class ThreadWithAssertion(threading.Thread):

    def __init__(self, target, channel_map):
        super().__init__(target=target, args=(channel_map, ))
        self.exc = None

    def run(self):
        try:
            super().run()
        except BaseException as e:
            self.exc = e

    def join(self, timeout=None):
        super().join(timeout)
        if self.exc:
            raise self.exc


def test_node():
    # Assertions within a thread won't automatically fail the pytest test
    # because they are raised in a different context. To correctly capture and
    # propagate these failures, a mechanism is needed to re-raise exceptions in
    # the main thread.  We subclass Thread to raise an exception on join, and
    # pass the thread callbacks in the main loop below.

    def node_a_callback(channel_map: ChannelMap):
        assert "node_a" in channel_map.nodes
        node_a = Node(config=channel_map.nodes["node_a"])

        # Access the message publisher
        node_a_int = node_a.publisher(channel_id="node_a_int")

        # Sleep before sending to give the receiver time to spin up and listen
        time.sleep(0.1)
        node_a_int.send(msg=Int64Value(value=11))

    def node_b_callback(channel_map: ChannelMap):
        assert "node_b" in channel_map.nodes
        node_b = Node(config=channel_map.nodes["node_b"])

        # Access the message subscriber
        node_a_int = node_b.subscriber(channel_id="node_a_int")

        # Listen for the message and check the correct value was received
        pb_int = Int64Value()
        assert node_a_int.recv(msg=pb_int) == True
        assert pb_int.value == 11

    channel_map = load_test_channel_map()

    node_a_thread = ThreadWithAssertion(
        target=node_a_callback,
        channel_map=channel_map,
    )
    node_b_thread = ThreadWithAssertion(
        target=node_b_callback,
        channel_map=channel_map,
    )

    node_a_thread.start()
    node_b_thread.start()

    node_a_thread.join()
    node_b_thread.join()
