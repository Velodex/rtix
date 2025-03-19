# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import logging
import yaml
from google.protobuf.wrappers_pb2 import BoolValue

from rtix.core.logging import setupDefaultLogging
from rtix.core.timer import Timer, getTimestampNs
from rtix.ipc.node import Node
from rtix.ipc.channel_map import ChannelMap

LOG_FILE = "/rtix/logs/py_node_a.log"
PING_CHANNEL = "ping"
PONG_CHANNEL = "pong"
MAX_COUNTS = 50
PUBLISH_FREQ_S = 0.1


def main():
    # This will log to both stdout and the specified logfile
    setupDefaultLogging(log_file=LOG_FILE)
    logging.info("Starting Node A")

    # Load the channel map
    with open("../channel-map.yaml") as file:
        yaml_dict = yaml.safe_load(file)
        channel_map = ChannelMap.LoadYaml(yaml_dict)

    # Create the node
    node = Node(config=channel_map.nodes["node_a"])

    # Wait for the containers to spin up
    Timer.Sleep(2.0)

    # Begin the publisher loop
    counter = 0
    done = False
    while not done:
        tic_ns = getTimestampNs()

        # Check if we should should wrap up the node
        done = counter >= MAX_COUNTS

        # Here we send a standard protobuf BoolValue message that contains a
        # flag value.  You can also use your own custom protobuf messages.
        msg = BoolValue(value=(not done))
        if not node.publisher(PING_CHANNEL).send(msg):
            logging.warning("Node A failed to send to 'ping'")
            return

        # Now we wait for the response from node b.
        if not node.subscriber(PONG_CHANNEL).recv(msg, block=True):
            logging.warning("Node A did not receive from 'pong'")
            return

        # Print out the round trip time (RTT)
        toc_ns = getTimestampNs()
        delta_ms = (toc_ns - tic_ns) / 1.0e6
        logging.info(f"Round trip time (RTT) {delta_ms:.2f} ms")

        # Wait to increment the counter and send the next message
        Timer.Sleep(PUBLISH_FREQ_S)
        counter += 1

    # The pub/sub classes automatically closes the socket on deletion.
    logging.info("Stopping Node A")


if __name__ == '__main__':
    main()
