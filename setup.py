# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import os
from typing import List
from setuptools import Command, setup, find_packages
from setuptools.command.build import build
import subprocess


class GenerateProtos(Command):
    """
    Generate protos.  The usual protobuf-distutils uses protoc, which can
    generate older/incompatible protos from the grpc_tools.protoc version.
    """
    user_options = []

    def initialize_options(self) -> None:
        pass

    def finalize_options(self) -> None:
        pass

    def run(self):
        protos = self.find_protobufs()
        for proto in protos:
            protoc_call = [
                "python3",
                "-m",
                "grpc_tools.protoc",
                "--proto_path=rtix/api",
                "--python_out=rtix/api",
                proto,
            ]
            subprocess.call(protoc_call)

    def find_protobufs(self) -> List[str]:
        proto_dir = "rtix/api"
        protos = []
        for file in os.listdir(proto_dir):
            if file.endswith(".proto"):
                protos.append(file)
        return protos


class CustomBuild(build):
    sub_commands = [('generate_protos', None)] + build.sub_commands


# Generate the bindings
# https://packaging.python.org/en/latest/guides/distributing-packages-using-setuptools/#setup-args
setup(
    name="rtix",
    version="0.9.3",
    author="Velodex Robotics, Inc and RTIX Developers",
    description=
    "Fast and lightweight IPC and orchestration layer for robotics and embodied AI applications.",
    packages=find_packages(include=["rtix", "rtix.*"]),
    package_data={"rtix.api": ["*.proto"]},
    cmdclass={
        "build": CustomBuild,
        "generate_protos": GenerateProtos
    },
    install_requires=[
        "grpcio>=1.70.0",
        "grpcio-tools>=1.70.0",
        "pynng>=0.7.1",
        "pyyaml>=6.0.2",
    ],
)
