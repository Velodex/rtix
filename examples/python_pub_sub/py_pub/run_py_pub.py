# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import logging
from google.protobuf.wrappers_pb2 import Int64Value

from rtix.core.logging import setupDefaultLogging
from rtix.core.timer import Timer
from rtix.ipc.node import Publisher

LOG_FILE = "/rtix/logs/py_pub.log"
CHANNEL_ID = "counter_channel"
MAX_COUNTS = 50
PUBLISH_FREQ_S = 0.1


def main():
    # This will log to both stdout and the specified logfile
    setupDefaultLogging(log_file=LOG_FILE)

    # Create the publisher at the channel
    config = Publisher.Config(channel_id=CHANNEL_ID)
    publisher = Publisher(config=config)

    # Wait for the listener container to spin up
    Timer.Sleep(2.0)

    # Begin the publisher loop
    counter = 0
    done = False
    while not done:
        # Here we send a standard protobuf Int64Value message that contains the
        # counter value.  You can also use your own custom protobuf messages.
        msg = Int64Value(value=counter)
        if not publisher.send(msg):
            logging.warning("Publisher failed to send")

        # Wait to increment the counter and send the next message
        Timer.Sleep(PUBLISH_FREQ_S)
        counter += 1

        # Check if we should should wrap up the publisher
        done = counter > MAX_COUNTS

    # The publisher class automatically closes the socket on deletion.
    logging.info("Closing the publisher")


if __name__ == '__main__':
    main()
