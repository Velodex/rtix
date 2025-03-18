# Copyright (c) 2025 Velodex Robotics, Inc and RTIX Developers.
# Licensed under Apache-2.0. http://www.apache.org/licenses/LICENSE-2.0

from rtix.core.timer import Timer


def test_timer():
    SLEEP_TOL_S = 1e-2

    timer = Timer()
    Timer.Sleep(1.0)
    et_s = timer.getElapsedS()
    assert abs(et_s - 1.0) < SLEEP_TOL_S
