// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#include <gtest/gtest.h>
#include <stdexcept>
#include "rtix/core/exception.h"

void nested_function() {
  throw std::runtime_error("Not a valid value");
}

TEST(Core, Exception) {
  try {
    nested_function();
  } catch (const std::exception& e) {
    std::string traceback = rtix::core::getFullTraceback();
    std::cout << traceback << std::endl;
  }
}
