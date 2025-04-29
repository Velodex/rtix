// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#include "rtix/core/logging.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

namespace rtix {
namespace core {

void setupDefaultLogging(const std::string& log_file,
                         spdlog::level::level_enum console_level,
                         spdlog::level::level_enum detailed_level,
                         const std::string& log_format,
                         bool truncate) {
  auto console_sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
  console_sink->set_pattern(log_format);
  console_sink->set_level(console_level);

  auto file_sink =
      std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file, truncate);
  file_sink->set_pattern(log_format);
  file_sink->set_level(detailed_level);

  spdlog::logger logger("multi_sink", {console_sink, file_sink});
  logger.set_level(detailed_level);
  spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));
  spdlog::set_pattern(log_format);
  spdlog::flush_on(spdlog::level::info);
}

}  // namespace core
}  // namespace rtix
