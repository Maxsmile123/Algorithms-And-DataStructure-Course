#include "sims.hpp"

#include <tf/rt/scheduler.hpp>

#include <wheels/test/framework.hpp>
#include <wheels/system/quick_exit.hpp>

void ReadyToDeadLock() {
  tf::rt::Scheduler::Current()->SetDeadlockHandler([] {
    std::cout << "Deadlock detected" << std::endl;
    wheels::QuickExit(0);  // World is broken, leave it ASAP
  });
}

TEST_SUITE(DeadLock) {
  TEST(SimOneFiber, wheels::test::TestOptions().ForceFork()) {
    tf::rt::Scheduler scheduler;

    scheduler.Run([] {
      ReadyToDeadLock();
      OneFiberDeadLock();
    });
  }

  TEST(SimTwoFibers, wheels::test::TestOptions().ForceFork()) {
    tf::rt::Scheduler scheduler;

    scheduler.Run([] {
      TwoFibersDeadLock();
    });
  }
}

RUN_ALL_TESTS()
