# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import logging
from rtix.core.logging import setupDefaultLogging


def test_logging():
    setupDefaultLogging("./py_test_log.log")

    logging.debug("Debug")
    logging.info("Info")
    logging.warning("Warning")
    logging.error("Error")
