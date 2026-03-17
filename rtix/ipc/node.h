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
    std::string address;

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
    std::string address;

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

/// Push publisher for sending messages to remote receivers using Push0/Pull0 protocol.
/// Unlike Publisher which listens locally, PushPublisher can dial remote TCP addresses.
/// Useful for sending commands to remote services that use Pull0 to receive messages.
class PushPublisher {
 public:
  struct Config {
    std::string channel_id;
    std::string address;  // Remote address to dial (e.g., "tcp://192.168.1.100:9001")
    bool send_raw_protobuf{false};  // If true, send raw protobuf without RTIX Packet wrapper

    static Config LoadYaml(const YAML::Node& yaml_node);
  };

  PushPublisher(const Config& config);
  virtual ~PushPublisher();
  bool send(const Message& msg) const;

 private:
  std::string _channel_id{};
  std::string _address{};
  bool _send_raw_protobuf{false};
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
