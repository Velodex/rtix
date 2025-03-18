// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include <yaml-cpp/yaml.h>
#include <unordered_map>
#include "rtix/ipc/node.h"

namespace rtix {
namespace ipc {

/// Metadata about connections to a specific channel
struct ChannelInfo {
  std::string channel_id;
  std::vector<std::string> publishers;
  std::vector<std::string> subscribers;
};

/// Source of truth for connections within the data plane
struct ChannelMap {
  std::unordered_map<std::string, Node::Config> nodes{};

  static ChannelMap LoadYaml(const YAML::Node& yaml_node);
  const Node::Config& getNodeConfig(const std::string& node_id) const;
  std::unordered_map<std::string, ChannelInfo> getChannelInfo() const;
  void validate() const;
};

}  // namespace ipc
}  // namespace rtix
