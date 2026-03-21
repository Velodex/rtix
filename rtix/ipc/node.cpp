// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#include "rtix/ipc/node.h"
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>
#include <nng/protocol/pipeline0/push.h>
#include <spdlog/spdlog.h>
#include <cstring>
#include "rtix/core/exception.h"
#include "rtix/core/timer.h"

namespace rtix {
namespace ipc {

const std::string CHANNELS_KEY = "channels";
const std::string TYPE_KEY = "type";
const std::string NODES_KEY = "nodes";
const std::string SUBCRIBERS_KEY = "subscribers";
const std::string PUBLISHERS_KEY = "publishers";
const std::string CHANNEL_KEY = "channel_id";
const std::string TIMEOUT_KEY = "timeout_ms";
const std::string MSG_PREFIX = "m";

std::string packMessage(const Message& msg) {
  rtix::types::common::Packet packet{};
  packet.mutable_payload()->PackFrom(msg);
  packet.mutable_metadata()->set_timestamp_ns(core::getTimestampNs());
  std::string data;
  packet.SerializeToString(&data);
  return MSG_PREFIX + data;
}

void unpackMessage(const std::string& data, Metadata& metadata, Message& msg) {
  rtix::types::common::Packet packet{};
  metadata.Clear();
  msg.Clear();
  std::size_t n = MSG_PREFIX.length();
  if (data.length() > n) {
    packet.ParseFromString(data.substr(1, data.length() - 1));
    metadata.CopyFrom(packet.metadata());
    packet.payload().UnpackTo(&msg);
  }
}

Publisher::Config Publisher::Config::LoadYaml(const YAML::Node& yaml_node) {
  Publisher::Config config{};
  config.channel_id = yaml_node[CHANNEL_KEY].as<std::string>();
  if (yaml_node["address"]) {
    config.address = yaml_node["address"].as<std::string>();
  }
  return config;
}

Publisher::Publisher(const Publisher::Config& config)
    : _channel_id(config.channel_id) {
  if (!config.address.empty()) {
    _address = config.address;
  } else {
    _address = std::string("ipc:///tmp/") + _channel_id + std::string(".ipc");
  }

  int rv;
  if ((rv = nng_pub0_open(&_socket)) != 0) {
    SPDLOG_ERROR("Pub '{}' nng_pub0_open failed ({})", _channel_id, rv);
    RTIX_THROW_IF_NOT(false, "IPC", "Failed to start publisher");
  }
  if ((rv = nng_listen(_socket, _address.c_str(), NULL, 0)) != 0) {
    SPDLOG_ERROR("Pub '{}' nng_listen failed ({})", _channel_id, rv);
    RTIX_THROW_IF_NOT(false, "IPC", "Failed to start publisher");
  }
  SPDLOG_INFO("Started publisher '{}' at {}", _channel_id, _address);
}

Publisher::~Publisher() {
  nng_close(_socket);
}

bool Publisher::send(const Message& msg) const {
  int rv;
  std::string d = packMessage(msg);
  if ((rv = nng_send(_socket, (void*)d.c_str(), d.length(), 0)) != 0) {
    SPDLOG_ERROR("Publisher '{}' send failed ({})", _channel_id, rv);
    return false;
  }
  SPDLOG_DEBUG("Publisher '{}' sent data", _channel_id);
  return true;
}

Subscriber::Config Subscriber::Config::LoadYaml(const YAML::Node& yaml_node) {
  Subscriber::Config config{};
  config.channel_id = yaml_node[CHANNEL_KEY].as<std::string>();
  config.timeout_ms = yaml_node[TIMEOUT_KEY].as<int>();
  if (yaml_node["address"]) {
    config.address = yaml_node["address"].as<std::string>();
  }
  return config;
}

Subscriber::Subscriber(const Subscriber::Config& config)
    : _channel_id(config.channel_id), _timeout_ms(config.timeout_ms) {
  if (!config.address.empty()) {
    _address = config.address;
  } else {
    _address = std::string("ipc:///tmp/") + _channel_id + std::string(".ipc");
  }
  
  int rv;
  if ((rv = nng_sub0_open(&_socket)) != 0) {
    SPDLOG_ERROR("Sub '{}' nng_sub0_open failed ({})", _channel_id, rv);
    RTIX_THROW_IF_NOT(false, "IPC", "Failed to start subscriber");
  }
  if ((rv = nng_setopt(_socket, NNG_OPT_SUB_SUBSCRIBE, "", 0)) != 0) {
    SPDLOG_ERROR("Sub '{}' nng_setopt failed ({})", _channel_id, rv);
    RTIX_THROW_IF_NOT(false, "IPC", "Failed to start subscriber");
  }
  if ((rv = nng_setopt_int(_socket, NNG_OPT_RECVBUF, 1)) != 0) {
    SPDLOG_ERROR("Sub '{}' nng_setopt_int failed ({})", _channel_id, rv);
    RTIX_THROW_IF_NOT(false, "IPC", "Failed to start subscriber");
  }
  if ((rv = nng_setopt_ms(_socket, NNG_OPT_RECVTIMEO, _timeout_ms)) != 0) {
    SPDLOG_ERROR("Sub '{}' nng_setopt_ms failed ({})", _channel_id, rv);
    RTIX_THROW_IF_NOT(false, "IPC", "Failed to start subscriber");
  }
  if ((rv = nng_dial(_socket, _address.c_str(), NULL, NNG_FLAG_NONBLOCK)) !=
      0) {
    SPDLOG_ERROR("Sub '{}' nng_dial failed ({})", _channel_id, rv);
    RTIX_THROW_IF_NOT(false, "IPC", "Failed to start subscriber");
  }
  SPDLOG_INFO("Started subscriber '{}' at {}", _channel_id, _address);
}

Subscriber::~Subscriber() {
  nng_close(_socket);
}

void Subscriber::flush() const {
  char* buf = NULL;
  size_t sz;
  nng_recv(_socket, &buf, &sz, NNG_FLAG_NONBLOCK);
  nng_free(buf, sz);
}

bool Subscriber::recv(Message& msg, bool block) const {
  Metadata metadata{};
  return recv(msg, metadata, block);
}

bool Subscriber::recv(Message& msg, Metadata& metadata, bool block) const {
  char* buf = NULL;
  size_t sz;
  int rv;
  int flags = NNG_FLAG_ALLOC;
  if (!block) {
    flags = NNG_FLAG_ALLOC | NNG_FLAG_NONBLOCK;
  }
  if ((rv = nng_recv(_socket, &buf, &sz, flags)) != 0) {
    if ((rv == NNG_ETIMEDOUT) || (rv == NNG_EAGAIN)) {
      return false;
    }
    SPDLOG_ERROR("Subscriber '{}' recv failed ({})", _channel_id, rv);
    return false;
  }
  std::string d = std::string(buf, sz);
  nng_free(buf, sz);
  unpackMessage(d, metadata, msg);
  SPDLOG_DEBUG("Subscriber '{}' received data", _channel_id);
  return true;
}

Node::Config Node::Config::LoadYaml(const YAML::Node& yaml_node) {
  Node::Config config{};
  for (const auto& pub : yaml_node[PUBLISHERS_KEY]) {
    config.publishers.push_back(Publisher::Config::LoadYaml(pub));
  }
  for (const auto& sub : yaml_node[SUBCRIBERS_KEY]) {
    config.subscribers.push_back(Subscriber::Config::LoadYaml(sub));
  }
  return config;
}

Node::Node(const Node::Config& config) {
  for (const auto& pub : config.publishers) {
    _pubs.emplace(pub.channel_id, std::make_shared<const Publisher>(pub));
  }

  // Subscribers are spun up second because the channel needs to exist
  for (const auto& sub : config.subscribers) {
    _subs.emplace(sub.channel_id, std::make_shared<const Subscriber>(sub));
  }
}

std::shared_ptr<const Publisher> Node::publisher(
    const std::string& channel_id) const {
  auto it = _pubs.find(channel_id);
  RTIX_THROW_IF_NOT(it != _pubs.end(), "IPC", "Publisher not found");
  return it->second;
}

std::shared_ptr<const Subscriber> Node::subscriber(
    const std::string& channel_id) const {
  auto it = _subs.find(channel_id);
  RTIX_THROW_IF_NOT(it != _subs.end(), "IPC", "Subscriber not found");
  return it->second;
}

PushPublisher::Config PushPublisher::Config::LoadYaml(const YAML::Node& yaml_node) {
  PushPublisher::Config config{};
  config.channel_id = yaml_node[CHANNEL_KEY].as<std::string>();
  if (yaml_node["address"]) {
    config.address = yaml_node["address"].as<std::string>();
  }
  if (yaml_node["send_raw_protobuf"]) {
    config.send_raw_protobuf = yaml_node["send_raw_protobuf"].as<bool>();
  }
  return config;
}

PushPublisher::PushPublisher(const PushPublisher::Config& config)
    : _channel_id(config.channel_id),
      _address(config.address),
      _send_raw_protobuf(config.send_raw_protobuf) {
  if (_address.empty()) {
    RTIX_THROW_IF_NOT(false, "IPC", "PushPublisher requires an address to dial");
  }

  int rv;
  if ((rv = nng_push0_open(&_socket)) != 0) {
    SPDLOG_ERROR("PushPub '{}' nng_push0_open failed ({})", _channel_id, rv);
    RTIX_THROW_IF_NOT(false, "IPC", "Failed to start push publisher");
  }
  
  // Dial the remote address (non-blocking)
  // NNG will retry in the background if connection isn't immediately available
  rv = nng_dial(_socket, _address.c_str(), NULL, NNG_FLAG_NONBLOCK);
  if (rv != 0 && rv != NNG_EAGAIN) {
    // Only fail on non-recoverable errors (not EAGAIN which means connection in progress)
    SPDLOG_WARN("PushPub '{}' nng_dial failed ({}) - connection may be established later", _channel_id, rv);
    // Don't throw - allow connection to be established later
  } else if (rv == 0) {
    SPDLOG_INFO("PushPub '{}' connected to {}", _channel_id, _address);
  } else {
    SPDLOG_INFO("PushPub '{}' dialing {} (connection in progress)", _channel_id, _address);
  }
}

PushPublisher::~PushPublisher() {
  nng_close(_socket);
}

bool PushPublisher::send(const Message& msg) const {
  int rv;
  std::string data;
  
  if (_send_raw_protobuf) {
    // Send raw protobuf serialization (for compatibility with non-RTIX receivers)
    if (!msg.SerializeToString(&data)) {
      SPDLOG_ERROR("PushPub '{}' failed to serialize message", _channel_id);
      return false;
    }
  } else {
    // Send wrapped in RTIX Packet format (standard RTIX behavior)
    data = packMessage(msg);
  }
  
  rv = nng_send(_socket, (void*)data.c_str(), data.length(), 0);
  if (rv != 0) {
    // Check for connection errors
    if (rv == NNG_ECLOSED || rv == NNG_ECONNREFUSED || rv == NNG_ECONNRESET) {
      SPDLOG_WARN("PushPub '{}' connection lost ({})", _channel_id, rv);
    } else if (rv == NNG_EAGAIN) {
      // Socket buffer full or connection still establishing - this is recoverable
      SPDLOG_DEBUG("PushPub '{}' send temporarily failed (EAGAIN)", _channel_id);
    } else {
      SPDLOG_DEBUG("PushPub '{}' send failed ({})", _channel_id, rv);
    }
    return false;
  }
  SPDLOG_DEBUG("PushPub '{}' sent data", _channel_id);
  return true;
}

}  // namespace ipc
}  // namespace rtix
