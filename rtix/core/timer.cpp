// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#include "rtix/core/timer.h"
#include <thread>

namespace rtix {
namespace core {

uint64_t getTimestampNs() {
  using namespace std::chrono;
  return duration_cast<nanoseconds>(system_clock::now().time_since_epoch())
      .count();
}

double nsToS(uint64_t time_ns) {
  return static_cast<double>(time_ns * 1e-9);
}

uint64_t nsToMs(uint64_t time_ns) {
  return static_cast<uint64_t>(time_ns * 1e-6);
}

uint64_t nsToUs(uint64_t time_ns) {
  return static_cast<uint64_t>(time_ns * 1e-3);
}

Timer::Timer() {
  _tic = std::chrono::steady_clock::now();
}

void Timer::start() {
  _tic = std::chrono::steady_clock::now();
}

double Timer::getElapsedS() const {
  auto toc = std::chrono::steady_clock::now();
  double et_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(toc - _tic).count();
  return et_ns / NS_PER_S;
}

uint64_t Timer::getElapsedNs() const {
  auto toc = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(toc - _tic)
      .count();
}

void Timer::Sleep(double duration_s) {
  std::this_thread::sleep_for(std::chrono::duration<double>(duration_s));
}

void Timer::Spinlock(double duration_s) {
  auto tic = std::chrono::steady_clock::now();
  double et_ns = 0;
  while (et_ns < duration_s * NS_PER_S) {
    auto toc = std::chrono::steady_clock::now();
    et_ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(toc - tic).count();
  }
}

}  // namespace core
}  // namespace rtix
