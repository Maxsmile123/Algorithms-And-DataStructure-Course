#include <wheels/test/framework.hpp>

#include <exe/executors/thread_pool.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sched/yield.hpp>
#include <exe/fibers/sync/event.hpp>

#include <wheels/test/util/cpu_timer.hpp>

#include <atomic>
#include <chrono>
#include <thread>

using namespace exe;

using namespace std::chrono_literals;

TEST_SUITE(Mutex) {
  SIMPLE_TEST(OneWaiter) {
    executors::ThreadPool scheduler{4};
    scheduler.Start();

    static const std::string kMessage = "Hello";

    fibers::Event event;
    std::string data;
    bool ok = false;

    fibers::Go(scheduler, [&] {
      event.Wait();
      ASSERT_EQ(data, kMessage);
      ok = true;
    });

    std::this_thread::sleep_for(1s);

    fibers::Go(scheduler, [&] {
      data = kMessage;
      event.Fire();
    });

    scheduler.WaitIdle();

    ASSERT_TRUE(ok);

    scheduler.Stop();
  }

  SIMPLE_TEST(DoNotBlockThread) {
    executors::ThreadPool scheduler{1};
    scheduler.Start();

    fibers::Event event;
    bool ok = false;

    fibers::Go(scheduler, [&] {
      event.Wait();
      ok = true;
    });

    fibers::Go(scheduler, [&] {
      for (size_t i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(32ms);
        fibers::Yield();
      }
      event.Fire();
    });

    scheduler.WaitIdle();

    ASSERT_TRUE(ok);

    scheduler.Stop();
  }

  SIMPLE_TEST(MultipleWaiters) {
    executors::ThreadPool scheduler{1};
    scheduler.Start();

    fibers::Event event;
    std::atomic<size_t> waiters{0};

    static const size_t kWaiters = 7;

    for (size_t i = 0; i < kWaiters; ++i) {
      fibers::Go(scheduler, [&] {
        event.Wait();
        ++waiters;
      });
    }

    std::this_thread::sleep_for(1s);

    fibers::Go(scheduler, [&] {
      event.Fire();
    });

    scheduler.WaitIdle();

    ASSERT_EQ(waiters.load(), kWaiters);

    scheduler.Stop();
  }

  SIMPLE_TEST(DoNotWasteCpu) {
    executors::ThreadPool scheduler{4};
    scheduler.Start();

    wheels::ProcessCPUTimer cpu_timer;

    fibers::Event event;

    fibers::Go(scheduler, [&] {
      event.Wait();
    });

    std::this_thread::sleep_for(1s);

    fibers::Go(scheduler, [&] {
      event.Fire();
    });

    scheduler.WaitIdle();

    ASSERT_TRUE(cpu_timer.Spent() < 100ms);

    scheduler.Stop();
  }
}

RUN_ALL_TESTS()
