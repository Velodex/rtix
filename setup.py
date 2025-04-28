# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import os
from typing import List
from setuptools import setup, find_packages
from setuptools_protobuf import Protobuf


def find_protobufs() -> List[Protobuf]:
    proto_dir = "rtix/api"
    protos = []
    for file in os.listdir(proto_dir):
        if file.endswith(".proto"):
            protos.append(Protobuf(os.path.join(proto_dir, file)))
    return protos


# Generate the bindings
# https://packaging.python.org/en/latest/guides/distributing-packages-using-setuptools/#setup-args
setup(
    name="rtix",
    version="0.9.1",
    author="Velodex Robotics, Inc and RTIX Developers",
    description=
    "Fast and lightweight IPC and orchestration layer for robotics and embodied AI applications.",
    packages=find_packages(include=["rtix", "rtix.*"]),
    package_data={"rtix.api": ["*.proto"]},
    setup_requires=["setuptools-protobuf"],
    protobufs=find_protobufs(),
    install_requires=[
        "grpcio>=1.70.0",
        "grpcio-tools>=1.70.0",
        "pynng>=0.7.1",
        "pyyaml>=6.0.2",
    ],
)
