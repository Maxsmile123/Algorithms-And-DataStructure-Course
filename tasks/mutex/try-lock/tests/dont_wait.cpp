#include "../ticket_lock.hpp"

#include <wheels/test/framework.hpp>

#include <twist/rt/run.hpp>
#include <twist/rt/layer/fiber/runtime/scheduler.hpp>

#include <twist/ed/stdlike/thread.hpp>

#include <twist/test/repeat.hpp>

#include <fmt/core.h>

#include <chrono>

using namespace std::chrono_literals;

// Assume TWIST_FIBERS=ON

TEST_SUITE(TicketTryLock) {
  SIMPLE_TEST(DontWaitInTryLock) {
    auto params = twist::rt::Params{}.TimeBudget(3s);

    twist::rt::Run(params, [] {
      auto* scheduler = twist::rt::fiber::Scheduler::Current();

      twist::test::Repeat repeat;

      while (repeat()) {
        size_t before = scheduler->SwitchCount();

        {
          TicketLock ticket_lock;

          twist::ed::stdlike::thread locker([&] {
            ticket_lock.Lock();
            twist::ed::stdlike::this_thread::sleep_for(1s);
            ticket_lock.Unlock();
          });

          {
            // Don't call Lock from TryLock
            if (ticket_lock.TryLock()) {
              ticket_lock.Unlock();
            }
          }

          locker.join();
        }

        size_t after = scheduler->SwitchCount();

        size_t switch_count = after - before;

        ASSERT_TRUE(switch_count < 37);
      }

      fmt::println("Test iterations: {}", repeat.IterCount());
    });
  }
}

RUN_ALL_TESTS()
