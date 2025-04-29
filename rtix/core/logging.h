// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include <spdlog/spdlog.h>

namespace rtix {
namespace core {

const std::string DETAILED_LOG_FMT = "[%Y-%m-%d %H:%M:%S,%e] [%s:%#] [%l] %v";

/// Initializes a default logger, writing to stdout and file
///
/// NOTE: If using compile-time spdlog macros, e.g.: SPDLOG_INFO(...), you will
/// need to add the following to your project CMakeLists.txt, where the active
/// level is your minimum log level that you intend to use, e.g. debug:
/// add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
void setupDefaultLogging(
    const std::string& log_file,
    spdlog::level::level_enum console_level = spdlog::level::info,
    spdlog::level::level_enum detailed_level = spdlog::level::debug,
    const std::string& log_format = DETAILED_LOG_FMT,
    bool truncate = true);

}  // namespace core
}  // namespace rtix
