# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import time


def getTimestampNs():
    """Returns the current time since epoch (ns)"""
    return time.time_ns()


def nsToS(time_ns: int) -> float:
    """Convert nanoseconds to seconds"""
    return float(time_ns * 1e-9)


def nsToMs(time_ns: int) -> int:
    """Convert nanoseconds to milliseconds"""
    return int(time_ns * 1e-6)


def nsToUs(time_ns: int) -> int:
    """Convert nanoseconds to microseconds"""
    return int(time_ns * 1e-3)


class Timer:
    """
    A timer to keep track of elapsed time and sleep the thread.

    NOTE: Unlike the C++ version, the Python timer does not provide an option
    to spinlock the thread because Python processes should not be expected to
    perform with this level of precision.  If such precision is needed, use C++
    """
    MS_PER_S = 1e3
    US_PER_S = 1e6
    NS_PER_S = 1e9

    def __init__(self):
        self.start()

    def start(self):
        self._tic_ns = getTimestampNs()

    def getElapsedS(self) -> float:
        toc_ns = getTimestampNs()
        return nsToS(toc_ns - self._tic_ns)

    def getElapsedNs(self) -> int:
        toc_ns = getTimestampNs()
        return toc_ns - self._tic_ns

    @staticmethod
    def Sleep(duration_s: float):
        time.sleep(duration_s)
