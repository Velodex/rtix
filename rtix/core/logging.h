// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include <spdlog/spdlog.h>

namespace rtix {
namespace core {

/// Initializes a default logger, writing to stdout and file
void setupDefaultLogging(const std::string& log_file,
                         spdlog::level::level_enum level = spdlog::level::info);

}  // namespace core
}  // namespace rtix
