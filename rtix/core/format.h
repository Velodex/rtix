// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include <sstream>

namespace rtix {
namespace core {

/// Formats an expression inplace using stringstream
#define RTIX_FMT(expression) \
  ((std::ostringstream&)(std::ostringstream() << expression)).str()

}  // namespace core
}  // namespace rtix
