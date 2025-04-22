# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

import yaml

from rtix.ipc.channel_map import ChannelMap

IPC_TEST_YAML_FILE = "/rtix/rtix/ipc/tests/channel-map.yaml"


def load_test_channel_map(yaml_file: str = IPC_TEST_YAML_FILE) -> ChannelMap:
    with open(yaml_file) as stream:
        yaml_dict = yaml.safe_load(stream)
        return ChannelMap.LoadYaml(yaml_dict=yaml_dict)


def test_channel_map():
    channel_map = load_test_channel_map()
    assert channel_map is not None
    assert len(channel_map.nodes) == 2
    assert "node_a" in channel_map.nodes
    assert "node_b" in channel_map.nodes
