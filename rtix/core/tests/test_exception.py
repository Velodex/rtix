# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

from rtix.core.exception import getFullTraceback


def nested_function():
    raise ValueError("Not a valid value")


def test_exception():
    try:
        nested_function()
    except Exception as e:
        traceback = getFullTraceback()
        assert traceback != ""
        print(traceback)


if __name__ == "__main__":
    test_exception()
