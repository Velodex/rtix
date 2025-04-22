// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#include "rtix/core/exception.h"
#include <cpptrace/cpptrace.hpp>

namespace rtix {
namespace core {

std::string getFullTraceback() {
  auto stacktrace = cpptrace::generate_trace();
  return stacktrace.to_string(true);
}

}  // namespace core
}  // namespace rtix
