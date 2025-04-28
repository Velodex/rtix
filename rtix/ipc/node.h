// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include <google/protobuf/message.h>
#include <nng/nng.h>
#include <yaml-cpp/yaml.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include "rtix/types/common.pb.h"

namespace rtix {
namespace ipc {

using google::protobuf::Message;
using rtix::types::common::Metadata;

/// Packs the protobuf message into a binary data packet with metadata
std::string packMessage(const Message& msg);

/// Unpacks a binary data packet into a protobuf message and metadata
void unpackMessage(const std::string& data, Metadata& metadata, Message& msg);

/// Primary interface for publishing messages to a channel
class Publisher {
 public:
  struct Config {
    std::string channel_id;

    static Config LoadYaml(const YAML::Node& yaml_node);
  };

  Publisher(const Config& config);
  virtual ~Publisher();
  bool send(const Message& msg) const;

 private:
  std::string _channel_id{};
  std::string _address{};
  nng_socket _socket;
};

/// Primary interface for subscribing to messages from a channel
class Subscriber {
 public:
  struct Config {
    std::string channel_id;
    int timeout_ms;

    static Config LoadYaml(const YAML::Node& yaml_node);
  };

  Subscriber(const Config& config);
  virtual ~Subscriber();
  void flush() const;
  bool recv(Message& msg, bool block = true) const;
  bool recv(Message& msg, Metadata& metadata, bool block = true) const;

 private:
  std::string _channel_id{};
  int _timeout_ms{};
  std::string _address{};
  nng_socket _socket;
};

/// Primary interface to manage multiple pub/sub within a single process
class Node {
 public:
  struct Config {
    std::vector<Publisher::Config> publishers;
    std::vector<Subscriber::Config> subscribers;

    static Config LoadYaml(const YAML::Node& yaml_node);
  };

  Node(const Config& config);
  virtual ~Node() = default;
  std::shared_ptr<const Publisher> publisher(
      const std::string& channel_id) const;
  std::shared_ptr<const Subscriber> subscriber(
      const std::string& channel_id) const;

 private:
  std::unordered_map<std::string, std::shared_ptr<const Publisher>> _pubs{};
  std::unordered_map<std::string, std::shared_ptr<const Subscriber>> _subs{};
};

}  // namespace ipc
}  // namespace rtix
