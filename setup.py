# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import os
import re
from setuptools import setup, find_packages

# File path
FILE_PATH = os.path.dirname(os.path.abspath(__file__))


# Get the project version
def get_version() -> str:
    with open(os.path.join(FILE_PATH, "version.txt")) as f:
        content = f.readlines()
    vdict = {}
    for x in content:
        pattern = "(\S*) (\d[0-9]*)"
        match = re.search(pattern, x.strip())
        vdict[match.group(1)] = match.group(2)
    ver = "{}.{}.{}".format(vdict["VERSION_MAJOR"], vdict["VERSION_MINOR"],
                            vdict["VERSION_PATCH"])
    return ver


# Generate the bindings
# https://packaging.python.org/en/latest/guides/distributing-packages-using-setuptools/#setup-args
setup(
    name="rtix",
    version=get_version(),
    author="Velodex Robotics, Inc and RTIX Developers",
    description=
    "Fast and lightweight IPC and orchestration layer for robotics and embodied AI applications.",
    packages=find_packages(include=["rtix", "rtix.*"]),
    install_requires=[
        "grpcio>=1.70.0",
        "grpcio-tools>=1.70.0",
        "pynng>=0.7.1",
        "pyyaml>=6.0.2",
    ],
)
