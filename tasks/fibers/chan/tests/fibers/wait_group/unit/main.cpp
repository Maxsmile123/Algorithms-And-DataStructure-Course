#include <exe/executors/thread_pool.hpp>

#include <exe/fibers/core/api.hpp>
#include <exe/fibers/sync/wait_group.hpp>

#include <wheels/test/framework.hpp>
#include <wheels/support/cpu_time.hpp>

#include <atomic>
#include <thread>

using namespace exe;

TEST_SUITE(WaitGroup) {
  SIMPLE_TEST(OneWaiter) {
    executors::ThreadPool scheduler{/*threads=*/5};

    fibers::WaitGroup wg;
    std::atomic<size_t> workers{0};
    std::atomic<bool> waiter{false};

    static const size_t kWorkers = 3;

    wg.Add(kWorkers);

    fibers::Go(scheduler, [&]() {
      wg.Wait();
      ASSERT_EQ(workers.load(), kWorkers);
      waiter = true;
    });

    for (size_t i = 0; i < kWorkers; ++i) {
      fibers::Go(scheduler, [&]() {
        std::this_thread::sleep_for(1s);
        ++workers;
        wg.Done();
      });
    }

    scheduler.WaitIdle();

    ASSERT_TRUE(waiter);

    scheduler.Stop();
  }

  SIMPLE_TEST(MultipleWaiters) {
    executors::ThreadPool scheduler{/*threads=*/5};

    fibers::WaitGroup wg;

    std::atomic<size_t> workers{0};
    std::atomic<size_t> waiters{0};

    static const size_t kWorkers = 3;
    static const size_t kWaiters = 4;

    wg.Add(kWorkers);

    for (size_t i = 0; i < kWaiters; ++i) {
      fibers::Go(scheduler, [&]() {
        wg.Wait();
        ASSERT_EQ(workers.load(), kWorkers);
        ++waiters;
      });
    }

    for (size_t i = 0; i < kWorkers; ++i) {
      fibers::Go(scheduler, [&]() {
        std::this_thread::sleep_for(1s);
        ++workers;
        wg.Done();
      });
    }

    scheduler.WaitIdle();

    ASSERT_EQ(waiters, kWaiters);

    scheduler.Stop();
  }

  SIMPLE_TEST(BlockingWait) {
    executors::ThreadPool scheduler{/*threads=*/4};

    fibers::WaitGroup wg;

    std::atomic<size_t> workers = 0;

    wheels::ProcessCPUTimer timer;

    static const size_t kWorkers = 3;

    wg.Add(kWorkers);

    fibers::Go(scheduler, [&]() {
      wg.Wait();
      ASSERT_EQ(workers.load(), kWorkers);
    });

    for (size_t i = 0; i < kWorkers; ++i) {
      fibers::Go(scheduler, [&]() {
        std::this_thread::sleep_for(1s);
        ++workers;
        wg.Done();
      });
    }

    scheduler.WaitIdle();

    ASSERT_TRUE(timer.Elapsed() < 100ms);

    scheduler.Stop();
  }
}

RUN_ALL_TESTS()
