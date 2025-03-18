# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import sys
import logging


def setupDefaultLogging(log_file: str, level: int = logging.INFO):
    """Initializes a default logger, writing to stdout and file"""
    logging.basicConfig(
        format="%(asctime)s | %(levelname)8s | %(message)s",
        level=level,
        handlers=[
            logging.FileHandler(log_file),
            logging.StreamHandler(sys.stdout),
        ],
    )
