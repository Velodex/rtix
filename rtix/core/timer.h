// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include <chrono>

namespace rtix {
namespace core {

/// Returns the current time since epoch (ns)
uint64_t getTimestampNs();

/// A timer to keep track of elapsed time and sleep/spinlock the thread
class Timer {
 public:
  static constexpr double MS_PER_S = 1e3;
  static constexpr double US_PER_S = 1e6;
  static constexpr double NS_PER_S = 1e9;

  Timer();
  virtual ~Timer() = default;
  void start();
  double getElapsedS() const;
  unsigned getElapsedNs() const;
  static void Sleep(double duration_s);
  static void Spinlock(double duration_s);

 private:
  std::chrono::steady_clock::time_point _tic;
};

}  // namespace core
}  // namespace rtix
