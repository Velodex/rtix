# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import sys
import logging

SIMPLE_LOG_FMT = "[%(asctime)s] [%(levelname)s] %(message)s"
DETAILED_LOG_FMT = "[%(asctime)s] [%(filename)s:%(lineno)d] [%(levelname)s] %(message)s"


def setupDefaultLogging(
    log_file: str,
    console_level: int = logging.INFO,
    detailed_level: int = logging.DEBUG,
    log_format: str = DETAILED_LOG_FMT,
    truncate: bool = False,
):
    """Initializes a default logger, writing to stdout and file"""
    logger = logging.getLogger()
    logger.setLevel(detailed_level)

    formatter = logging.Formatter(log_format)

    mode = "w" if truncate else "a"
    stream_handler = logging.StreamHandler(sys.stdout, mode=mode)
    stream_handler.setLevel(console_level)
    stream_handler.setFormatter(formatter)
    logger.addHandler(stream_handler)

    file_handler = logging.FileHandler(log_file)
    file_handler.setLevel(detailed_level)
    file_handler.setFormatter(formatter)
    logger.addHandler(file_handler)
