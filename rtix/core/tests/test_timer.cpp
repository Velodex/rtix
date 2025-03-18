// Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
// Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

#include <gtest/gtest.h>
#include "rtix/core/timer.h"

TEST(Core, Timer) {
  // Tolerances are processor specific.  If these tests do not pass on your
  // platform, you may want to consider upgrading your hardware.
  const double SPINLOCK_TOL_S = 1e-4;
  const double SLEEP_TOL_S = 1e-2;

  using rtix::core::Timer;

  Timer timer{};
  Timer::Spinlock(1.0);
  double et_s = timer.getElapsedS();
  EXPECT_NEAR(et_s, 1.0, SPINLOCK_TOL_S);

  timer.start();
  Timer::Sleep(1.0);
  et_s = timer.getElapsedS();
  EXPECT_NEAR(et_s, 1.0, SLEEP_TOL_S);
}
