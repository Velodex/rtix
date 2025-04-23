// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#include <google/protobuf/wrappers.pb.h>
#include <gtest/gtest.h>
#include "rtix/api/common.pb.h"
#include "rtix/core/timer.h"
#include "rtix/ipc/channel_map.h"
#include "rtix/ipc/node.h"
#include "rtix/process/process.h"

using google::protobuf::BoolValue;
using google::protobuf::Int64Value;
using rtix::api::common::Code;
using rtix::api::common::Status;
using rtix::core::Timer;
using rtix::ipc::ChannelMap;
using rtix::ipc::Node;
using rtix::process::InputConfig;
using rtix::process::OutputConfig;
using rtix::process::Process;
using rtix::process::ProcessConfig;

double MAX_PROCESS_TIME_S = 3.0;
bool ACTION_VALUE = true;
int INPUT_VALUE = 567;

ChannelMap load_test_channel_map() {
  std::string yaml_file = "/rtix/rtix/process/tests/channel-map.yaml";
  YAML::Node yaml_node = YAML::LoadFile(yaml_file);
  return ChannelMap::LoadYaml(yaml_node);
}

class TestProcess : public Process<BoolValue> {
 public:
  explicit TestProcess(const Node& node, const ProcessConfig& config)
      : Process<BoolValue>(node, config) {
    SPDLOG_INFO("Created test process");
  }
  virtual ~TestProcess() = default;

  void handleAction(const BoolValue& action) override {
    // Need to sleep to give the fixture node time to receive
    Timer::Sleep(0.2);
    publishStatus(Code::SUCCESS, "Handled");
    setRunning(true);
    SPDLOG_INFO("Handled action");
  }

  void stepOuter() override {
    if (receiveInput("in", input)) {
      SPDLOG_INFO("Received input");
    }

    if (getProcessTimeS() > MAX_PROCESS_TIME_S) {
      stop();
    }
  }

  void stepInner() override {
    publishOutput("out", input);
    SPDLOG_INFO(RTIX_FMT("Published output " << input.value()));
  }

 private:
  Int64Value input{};
};

void test_process_callback(ChannelMap channel_map) {
  EXPECT_TRUE(channel_map.nodes.find("test_node") != channel_map.nodes.end());
  auto test_node = Node(channel_map.getNodeConfig("test_node"));

  // Create the process
  ProcessConfig config;
  config.monitor_rate_s = 0.1;
  config.loop_rate_s = 0.05;
  config.action_channel_id = "action";
  config.status_channel_id = "status";
  config.input_channels["in"] = InputConfig{"x"};
  config.output_channels["out"] = OutputConfig{"y"};
  TestProcess test_process(test_node, config);

  // Run for finite amount of time
  test_process.run();
}

TEST(Process, Process) {
  const auto channel_map = load_test_channel_map();

  std::thread test_process_thread(test_process_callback, channel_map);

  // Access the fixture node after the test process has started
  EXPECT_TRUE(channel_map.nodes.find("fixture_node") !=
              channel_map.nodes.end());
  auto fixture_node = Node(channel_map.getNodeConfig("fixture_node"));

  // Give the thread time to spin up
  Timer::Sleep(0.2);

  // Send the action
  BoolValue action{};
  action.set_value(ACTION_VALUE);
  fixture_node.publisher("action")->send(action);

  // Await status
  Status status{};
  fixture_node.subscriber("status")->recv(status);

  // Send the 'x' channel
  Int64Value message{};
  message.set_value(INPUT_VALUE);
  fixture_node.publisher("x")->send(message);

  // Give the process time to return the input value
  Timer::Sleep(0.2);

  // Receive the 'y' channel
  Int64Value output{};
  fixture_node.subscriber("y")->recv(output);

  test_process_thread.join();

  // These will only be true if the input and action was received
  EXPECT_EQ(status.code(), Code::SUCCESS);
  EXPECT_EQ(output.value(), INPUT_VALUE);
}
