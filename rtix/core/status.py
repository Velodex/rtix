# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

from enum import Enum


class Status(Enum):
    """Enum for recording the status of an operation"""
    RUNNING = 0
    SUCCESS = 1
    FAILURE = 2
