// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#include "rtix/ipc/channel_map.h"
#include <iostream>
#include "rtix/core/exception.h"

namespace rtix {
namespace ipc {

ChannelMap ChannelMap::LoadYaml(const YAML::Node& yaml_node) {
  ChannelMap channel_map{};
  for (auto it = yaml_node.begin(); it != yaml_node.end(); ++it) {
    const std::string& key = it->first.as<std::string>();
    const YAML::Node& value = it->second;
    channel_map.nodes[key] = Node::Config::LoadYaml(value);
  }
  return channel_map;
}

const Node::Config& ChannelMap::getNodeConfig(
    const std::string& node_id) const {
  const auto it = nodes.find(node_id);
  RTIX_THROW_IF_NOT(it != nodes.end(), "IPC", "Node not found");
  return it->second;
}

std::unordered_map<std::string, ChannelInfo> ChannelMap::getChannelInfo()
    const {
  return std::unordered_map<std::string, ChannelInfo>{};
}

void ChannelMap::validate() const {}

}  // namespace ipc
}  // namespace rtix
