// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#include <gtest/gtest.h>
#include "rtix/core/format.h"

TEST(Core, Format) {
  std::string str = RTIX_FMT("This is formatted " << 10 << ".");
  EXPECT_EQ(str, "This is formatted 10.");
}
