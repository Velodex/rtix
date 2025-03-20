# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import sys
import logging


def setupDefaultLogging(log_file: str, level: int = logging.INFO):
    """Initializes a default logger, writing to stdout and file"""
    DETAILED_LOG_FMT = "[%(asctime)s] [%(filename)s:%(lineno)d] [%(levelname)s] %(message)s"
    DETAILED_LOG_LEVEL = logging.DEBUG

    logger = logging.getLogger()
    logger.setLevel(DETAILED_LOG_LEVEL)

    formatter = logging.Formatter(DETAILED_LOG_FMT)

    stream_handler = logging.StreamHandler(sys.stdout)
    stream_handler.setLevel(level)
    stream_handler.setFormatter(formatter)
    logger.addHandler(stream_handler)

    file_handler = logging.FileHandler(log_file)
    file_handler.setLevel(DETAILED_LOG_LEVEL)
    file_handler.setFormatter(formatter)
    logger.addHandler(file_handler)
