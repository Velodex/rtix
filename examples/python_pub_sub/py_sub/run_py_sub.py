# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import logging
from google.protobuf.wrappers_pb2 import Int64Value
from rtix.types.common_pb2 import Metadata

from rtix.core.logging import setupDefaultLogging
from rtix.core.timer import getTimestampNs, Timer
from rtix.ipc.node import Subscriber

LOG_FILE = "/rtix/logs/py_sub.log"
CHANNEL_ID = "counter_channel"
TIMEOUT_MS = 200
MAX_COUNTS = 50


def main():
    # This will log to both stdout and the specified logfile
    setupDefaultLogging(log_file=LOG_FILE)

    # Wait for the publisher container to spin up
    Timer.Sleep(1.0)

    # Create the subscriber at the channel
    config = Subscriber.Config(channel_id=CHANNEL_ID, timeout_ms=TIMEOUT_MS)
    subscriber = Subscriber(config=config)
    subscriber.flush()

    # Begin the subscriber loop
    done = False
    while not done:
        # Here we receice the same message type as is published.  The msg field
        # contains the raw data, and the metadata contains a timestamp of when
        # the originating message was published.
        msg = Int64Value()
        metadata = Metadata()
        if not subscriber.recv(msg, metadata=metadata):
            continue

        # Print the message and the timestamp difference which is the round
        # trip time (RTT).  RTT includes serialization, deserialization, and
        # communication latency.  For high performane RTT, use C++ over Python.
        rtt_ms = (getTimestampNs() - metadata.timestamp_ns) / 1e6
        logging.info(f"Received message: {msg.value} (RTT={rtt_ms:.2f}ms)")

        # Check if we should should wrap up the subscriber
        done = msg.value >= MAX_COUNTS

    # The subscriber class automatically closes the socket on deletion.
    logging.info("Closing the subscriber")


if __name__ == '__main__':
    main()
