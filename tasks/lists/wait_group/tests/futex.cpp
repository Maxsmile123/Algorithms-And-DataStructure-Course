#include "../wait_group.hpp"

#include <wheels/test/framework.hpp>

#include <twist/rt/run.hpp>
#include <twist/rt/layer/fiber/runtime/scheduler.hpp>

// Assume TWIST_FIBERS=ON

TEST_SUITE(WaitGroup) {
  SIMPLE_TEST(Futex) {
    twist::rt::Run([] {
      auto* scheduler = twist::rt::fiber::Scheduler::Current();

      {
        WaitGroup wg;

        wg.Add(128);

        for (size_t i = 0; i < 128; ++i) {
          wg.Done();
        }

        wg.Wait();
      }

      ASSERT_LT(scheduler->FutexCount(), 10);
    });
  }
}

RUN_ALL_TESTS()
