#include <wheels/test/framework.hpp>

#include <twist/rt/run.hpp>
#include <twist/rt/layer/fiber/runtime/scheduler.hpp>

#include "../mutex.hpp"

// Assume TWIST_FIBERS=ON

TEST_SUITE(Futex) {
  SIMPLE_TEST(NoContention) {
    twist::rt::Run([] {
      auto* scheduler = twist::rt::fiber::Scheduler::Current();

      {
        stdlike::Mutex mutex;

        mutex.Lock();
        mutex.Unlock();
      }

      // No contention => no system calls
      ASSERT_EQ(scheduler->FutexCount(), 0);
    });
  }
}

RUN_ALL_TESTS()
