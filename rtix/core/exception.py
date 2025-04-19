# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import traceback
import sys


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


def getFullTraceback() -> str:
    """Return the detailed full traceback in the event of an exception"""
    exc_type, exc_value, exc_traceback = sys.exc_info()

    # Format the traceback
    traceback_details = traceback.extract_tb(exc_traceback)
    formatted_traceback = "".join(traceback.format_list(traceback_details))

    # Format the exception details
    formatted_exception = "".join(
        traceback.format_exception_only(exc_type, exc_value)).strip()

    # Combine the traceback and exception details
    return f"{formatted_traceback}{formatted_exception}"
