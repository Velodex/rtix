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

Timer::Timer() {
  _tic = std::chrono::steady_clock::now();
}

void Timer::start() {
  _tic = std::chrono::steady_clock::now();
}

double Timer::getElapsedS() {
  auto toc = std::chrono::steady_clock::now();
  double et_us =
      std::chrono::duration_cast<std::chrono::microseconds>(toc - _tic).count();
  return et_us / US_PER_S;
}

unsigned Timer::getElapsedNs() {
  auto toc = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(toc - _tic)
      .count();
}

void Timer::Sleep(double duration_s) {
  std::this_thread::sleep_for(std::chrono::duration<double>(duration_s));
}

void Timer::Spinlock(double duration_s) {
  auto tic = std::chrono::steady_clock::now();
  double et_us = 0;
  while (et_us < duration_s * US_PER_S) {
    auto toc = std::chrono::steady_clock::now();
    et_us = std::chrono::duration_cast<std::chrono::microseconds>(toc - tic)
                .count();
  }
}

}  // namespace core
}  // namespace rtix
