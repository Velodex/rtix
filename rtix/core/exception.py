# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0


def RTIX_THROW(module: str, msg: str):
    """Throws an exception with the provided message"""
    what = module + ": " + msg
    raise RuntimeError(what)


def RTIX_THROW_IF(evaluation: bool, module: str, msg: str):
    """Throws an exception with the provided message if evaluation is true"""
    if evaluation:
        RTIX_THROW(module=module, msg=msg)


def RTIX_THROW_IF_NOT(evaluation: bool, module: str, msg: str):
    """Throws an exception with the provided message if evaluation is false"""
    RTIX_THROW_IF(not evaluation, module=module, msg=msg)
