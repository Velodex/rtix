// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#include <spdlog/spdlog.h>
#include "rtix/core/exception.h"
#include "rtix/core/format.h"
#include "rtix/process/process.h"

namespace rtix {
namespace process {

template <typename ActionT>
Process<ActionT>::Process(const Node& node, const ProcessConfig& config)
    : _node(node), _config(config) {}

template <typename ActionT>
void Process<ActionT>::run() {
  _alive = true;
  _timer.start();
  while (_alive) {
    try {
      Timer loop_timer{};

      // Process the inputs at the very beginning of the loop so that any new
      // actions can compute on the latest input information.
      stepOuter();

      // If available, handle a new action and reset the loop timer to avoid
      // counting the action processing time as an overrun.
      if (_hasNewAction()) {
        handleAction(_action);
        loop_timer.start();
      }

      // Run the step function and try to match the target process rate if
      // running, otherwise wait for a new action.
      if (_running) {
        stepInner();
        _matchTargetLoopRate(loop_timer.getElapsedS());
      } else {
        Timer::Sleep(_config.monitor_rate_s);
      }
    } catch (const std::exception& e) {
      SPDLOG_ERROR(core::getFullTraceback());
      publishStatus(FAILURE, RTIX_FMT("Exception " << e.what()));
      _running = false;
    } catch (...) {
      publishStatus(FAILURE, "Encountered unknown exception");
      _running = false;
    }
  }
}

template <typename ActionT>
void Process<ActionT>::stop() {
  _alive = false;
}

template <typename ActionT>
void Process<ActionT>::setRunning(bool running) {
  _running = running;
}

template <typename ActionT>
double Process<ActionT>::getProcessTimeS() const {
  return _timer.getElapsedS();
}

template <typename ActionT>
void Process<ActionT>::publishStatus(const Code& code,
                                     const std::string& msg) const {
  auto status = Status();
  status.set_code(code);
  status.set_message(msg);
  if (code == FAILURE) {
    SPDLOG_ERROR(msg);
  } else {
    SPDLOG_INFO(msg);
  }
  _node.publisher(_config.status_channel_id)->send(status);
}

template <typename ActionT>
void Process<ActionT>::publishOutput(const std::string& key,
                                     const Message& msg) const {
  auto it = _config.output_channels.find(key);
  RTIX_THROW_IF(it == _config.output_channels.end(), "Process",
                RTIX_FMT("Output key '" << key << "' not found in channels"))
  const auto& channel = it->second;
  _node.publisher(channel.channel_id)->send(msg);
}

template <typename ActionT>
bool Process<ActionT>::receiveInput(const std::string& key,
                                    Message& msg) const {
  auto it = _config.input_channels.find(key);
  RTIX_THROW_IF(it == _config.input_channels.end(), "Process",
                RTIX_FMT("Input key '" << key << "' not found in channels"));
  const auto& channel = it->second;
  bool block = false;
  return _node.subscriber(channel.channel_id)->recv(msg, block);
}

template <typename ActionT>
void Process<ActionT>::flushInput(const std::string& key) const {
  auto it = _config.input_channels.find(key);
  RTIX_THROW_IF(it == _config.input_channels.end(), "Process",
                RTIX_FMT("Input key '" << key << "' not found in channels"));
  const auto& channel = it->second;
  _node.subscriber(channel.channel_id)->flush();
}

template <typename ActionT>
bool Process<ActionT>::_hasNewAction() {
  bool block = false;
  return _node.subscriber(_config.action_channel_id)->recv(_action, block);
}

template <typename ActionT>
void Process<ActionT>::_matchTargetLoopRate(double elapsed_s) {
  double target_s = _config.loop_rate_s;
  if (elapsed_s >= target_s) {
    SPDLOG_DEBUG("Task overrun of {} s", elapsed_s - target_s);
  } else {
    Timer::Sleep(target_s - elapsed_s);
  }
}

}  // namespace process
}  // namespace rtix
