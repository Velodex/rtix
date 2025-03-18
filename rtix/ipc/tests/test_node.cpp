// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#include <google/protobuf/wrappers.pb.h>
#include <gtest/gtest.h>
#include <thread>
#include "rtix/core/timer.h"
#include "rtix/ipc/node.h"
#include "rtix/ipc/tests/test_channel_map.h"

using google::protobuf::Int64Value;
using rtix::core::Timer;
using rtix::ipc::ChannelMap;
using rtix::ipc::Node;

void node_a_callback(ChannelMap channel_map) {
  EXPECT_TRUE(channel_map.nodes.find("node_a") != channel_map.nodes.end());
  auto node_a = Node(channel_map.getNodeConfig("node_a"));

  // Access the message publisher
  auto node_a_int = node_a.publisher("node_a_int");

  // Sleep before sending to give the receiver time to spin up and listen
  Timer::Sleep(0.5);

  Int64Value pb_int{};
  pb_int.set_value(11);
  node_a_int->send(pb_int);
  Timer::Sleep(0.5);
  node_a_int->send(pb_int);
}

void node_b_callback(ChannelMap channel_map) {
  EXPECT_TRUE(channel_map.nodes.find("node_b") != channel_map.nodes.end());
  auto node_b = Node(channel_map.getNodeConfig("node_b"));

  // Access the message subscriber
  auto node_a_int = node_b.subscriber("node_a_int");

  // Listen for the message and check the correct value was received
  Int64Value pb_int{};
  EXPECT_TRUE(node_a_int->recv(pb_int));
  EXPECT_EQ(pb_int.value(), 11);
}

TEST(Ipc, Node) {
  auto channel_map = load_test_channel_map();

  std::thread node_a_thread(node_a_callback, channel_map);
  std::thread node_b_thread(node_b_callback, channel_map);

  node_a_thread.join();
  node_b_thread.join();
}
