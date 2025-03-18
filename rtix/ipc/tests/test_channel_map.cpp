// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#include "rtix/ipc/tests/test_channel_map.h"
#include <gtest/gtest.h>
#include "rtix/ipc/channel_map.h"

using rtix::ipc::ChannelMap;

ChannelMap load_test_channel_map() {
  std::string yaml_file = "/rtix/rtix/ipc/tests/channel-map.yaml";
  YAML::Node yaml_node = YAML::LoadFile(yaml_file);
  return ChannelMap::LoadYaml(yaml_node);
}

TEST(Ipc, ChannelMap) {
  const auto channel_map = load_test_channel_map();
  EXPECT_EQ(channel_map.nodes.size(), 2);
  EXPECT_TRUE(channel_map.nodes.find("node_a") != channel_map.nodes.end());
  EXPECT_TRUE(channel_map.nodes.find("node_b") != channel_map.nodes.end());
}
