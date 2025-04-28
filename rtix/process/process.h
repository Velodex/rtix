// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include <google/protobuf/message.h>
#include <unordered_map>
#include "rtix/core/status.h"
#include "rtix/core/timer.h"
#include "rtix/ipc/node.h"

namespace rtix {
namespace process {

using google::protobuf::Message;
using rtix::core::Status;
using rtix::core::Timer;
using rtix::ipc::Node;

/// Configuration for input channels
struct InputConfig {
  std::string channel_id{};
};

/// Configuration for output channels
struct OutputConfig {
  std::string channel_id{};
};

/// Base process configuration
///
/// Attributes:
///     monitor_rate_s: The update rate when listening for an action
///     loop_rate_s: The update rate when the loop is running
///     action_channel_id: A channel for receiving actions from an orchestrator
///     status_channel_id: A channel for returning status to an orchestrator
///     input_channels: Input data channels (non-orchestrating)
///     output_channels: Output data channels (non-orchestrating)
struct ProcessConfig {
  double monitor_rate_s{};
  double loop_rate_s{};
  std::string action_channel_id{};
  std::string status_channel_id{};
  std::unordered_map<std::string, InputConfig> input_channels{};
  std::unordered_map<std::string, OutputConfig> output_channels{};
};

/// A class that responds to actions and publishes products and status.
/// Protected methods are implemented in the derived class,  the start()
/// function begins the process main loop.
template <typename ActionT>
class Process {
 public:
  Process(const Node& node, const ProcessConfig& config);
  virtual ~Process() = default;
  void run();
  void stop();

 protected:
  /// Implement: This function is called after a new action is received.
  virtual void handleAction(const ActionT& action) = 0;

  /// Implement: This function is called each iteration at the monitor_rate_s
  /// (if not running) or the loop_rate_s (if running).  This is where input
  /// information should be handled.
  virtual void stepOuter() = 0;

  /// Implement: This function is called each iteration at the loop_rate_s (if
  /// running), and is not called if not running.  This is where runtime
  /// computation should be handled.
  virtual void stepInner() = 0;

  void setRunning(bool running);
  double getProcessTimeS() const;
  void publishStatus(const Status& status, const std::string& msg) const;
  void publishOutput(const std::string& key, const Message& msg) const;
  bool receiveInput(const std::string& key, Message& msg) const;
  void flushInput(const std::string& key) const;

 protected:
  const Node& _node;
  const ProcessConfig _config{};

 private:
  bool _running{false};
  bool _alive{false};
  Timer _timer{};
  ActionT _action{};
  bool _hasNewAction();
  void _matchTargetLoopRate(double elapsed_s);
};

}  // namespace process
}  // namespace rtix

#include "rtix/process/process_impl.h"
