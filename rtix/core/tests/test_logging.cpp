// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include "rtix/core/logging.h"

TEST(Core, Logging) {
  rtix::core::setupDefaultLogging("./cpp_test_log.log");

  SPDLOG_DEBUG("Debug");
  SPDLOG_INFO("Info");
  SPDLOG_WARN("Warning");
  SPDLOG_ERROR("Error");
}
