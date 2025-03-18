# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

from __future__ import annotations
from typing import Dict, List
from dataclasses import dataclass

from rtix.ipc.node import Node


@dataclass
class ChannelInfo:
    """Metadata about connections to a specific channel"""
    channel_id: str
    publishers: List[str]
    subscribers: List[str]


@dataclass
class ChannelMap:
    """Source of truth for connections within the data plane"""
    nodes: Dict[str, Node.Config]

    @staticmethod
    def LoadYaml(yaml_dict: str) -> ChannelMap:
        """Creates the config object loaded from YAML"""
        nodes = {}
        for key, value in yaml_dict.items():
            nodes[key] = Node.Config.LoadYaml(yaml_dict=value)
        return ChannelMap(nodes=nodes)

    def getNodeConfig(self, node_id: str) -> Node.Config:
        """Returns the node config"""
        return self.nodes[node_id]

    def getChannelInfo(self) -> Dict[str, ChannelInfo]:
        """
        Parses the nodes to return information for the set of channels present.
        """
        raise NotImplementedError

    def validate(self):
        """
        Validates the channel map, mainly that there is one publisher per
        channel and that references exist.  Throws an exception on failure.
        """
        raise NotImplementedError
