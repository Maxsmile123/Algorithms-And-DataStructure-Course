#include <wheels/test/framework.hpp>

#include "sim.hpp"

#include <tf/rt/scheduler.hpp>

#include <wheels/system/quick_exit.hpp>

TEST_SUITE(LiveLock) {
  TEST(Sim, wheels::test::TestOptions().ForceFork()) {
    tf::rt::Scheduler scheduler;

    // Limit number of scheduler run loop iterations
    scheduler.Run([] { LiveLock(); }, /*fuel=*/123456);

    // World is broken, leave it ASAP
    wheels::QuickExit(0);
  }
}

RUN_ALL_TESTS()
