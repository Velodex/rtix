// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include <stdexcept>
#include <string>

namespace rtix {
namespace core {

/// Throws an exception with the provided message
#define RTIX_THROW(module, msg)                        \
  std::string what = module + std::string(": ") + msg; \
  throw std::runtime_error(what);

/// Throws an exception with the provided message if evaluation is true
#define RTIX_THROW_IF(evaluation, module, msg) \
  if (evaluation) {                            \
    RTIX_THROW(module, msg);                   \
  }

/// Throws an exception with the provided message if evaluation is false
#define RTIX_THROW_IF_NOT(evaluation, module, msg) \
  RTIX_THROW_IF(!(evaluation), module, msg)

}  // namespace core
}  // namespace rtix
