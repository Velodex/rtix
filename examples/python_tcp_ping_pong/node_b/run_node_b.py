# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import logging
import yaml
from google.protobuf.wrappers_pb2 import BoolValue

from rtix.core.logging import setupDefaultLogging
from rtix.core.timer import Timer
from rtix.ipc.node import Node
from rtix.ipc.channel_map import ChannelMap

LOG_FILE = "/rtix/logs/py_node_b.log"
PING_CHANNEL = "ping"
PONG_CHANNEL = "pong"


def main():
    # This will log to both stdout and the specified logfile
    setupDefaultLogging(log_file=LOG_FILE)
    logging.info("Starting Node B")

    # Load the channel map
    with open("../channel-map.yaml") as file:
        yaml_dict = yaml.safe_load(file)
        channel_map = ChannelMap.LoadYaml(yaml_dict)

    # Create the node
    node = Node(config=channel_map.nodes["node_b"])

    # Wait for the containers to spin up
    Timer.Sleep(2.0)

    # Begin the subscriber loop
    done = False
    while not done:
        # Receive the message published on the ping channel by Node A.
        msg = BoolValue()
        if not node.subscriber(PING_CHANNEL).recv(msg, block=True):
            logging.warning("Node B did not receive from 'ping'")
            return

        # Now we'll send the message back to Node A on the pong channel.
        if not node.publisher(PONG_CHANNEL).send(msg):
            logging.warning("Node A failed to send to 'ping'")
            return

        # Check if we should should wrap up the subscriber
        done = (not msg.value)

    # The pub/sub classes automatically closes the socket on deletion.
    logging.info("Stopping Node B")


if __name__ == '__main__':
    main()
