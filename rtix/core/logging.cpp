// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#include "rtix/core/logging.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

namespace rtix {
namespace core {

void setupDefaultLogging(const std::string& log_file,
                         spdlog::level::level_enum level) {
  const std::string LOG_FMT = "[%Y-%m-%d %H:%M:%S,%e] [%s:%#] [%l] %v";
  spdlog::level::level_enum LOG_LEVEL = spdlog::level::debug;

  auto console_sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
  console_sink->set_pattern(LOG_FMT);
  console_sink->set_level(level);

  auto file_sink =
      std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file, true);
  file_sink->set_pattern(LOG_FMT);
  file_sink->set_level(LOG_LEVEL);

  spdlog::logger logger("multi_sink", {console_sink, file_sink});
  logger.set_level(LOG_LEVEL);
  spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));
  spdlog::set_pattern(LOG_FMT);
}

}  // namespace core
}  // namespace rtix
