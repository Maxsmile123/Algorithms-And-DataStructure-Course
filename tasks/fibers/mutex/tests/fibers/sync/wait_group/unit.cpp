#include <wheels/test/framework.hpp>

#include <exe/executors/thread_pool.hpp>
#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sched/yield.hpp>
#include <exe/fibers/sync/wait_group.hpp>

#include <wheels/test/util/cpu_timer.hpp>

#include <atomic>
#include <thread>

using namespace exe;

TEST_SUITE(WaitGroup) {
  SIMPLE_TEST(OneWaiter) {
    executors::ThreadPool scheduler{/*threads=*/5};
    scheduler.Start();

    fibers::WaitGroup wg;
    std::atomic<size_t> work{0};
    bool ok = false;

    static const size_t kWorkers = 3;

    wg.Add(kWorkers);

    fibers::Go(scheduler, [&] {
      wg.Wait();
      ASSERT_EQ(work.load(), kWorkers);
      ok = true;
    });

    for (size_t i = 0; i < kWorkers; ++i) {
      fibers::Go(scheduler, [&] {
        std::this_thread::sleep_for(1s);
        ++work;
        wg.Done();
      });
    }

    scheduler.WaitIdle();

    ASSERT_TRUE(ok);

    scheduler.Stop();
  }

  SIMPLE_TEST(MultipleWaiters) {
    executors::ThreadPool scheduler{/*threads=*/5};
    scheduler.Start();

    fibers::WaitGroup wg;

    std::atomic<size_t> work{0};
    std::atomic<size_t> acks{0};

    static const size_t kWorkers = 3;
    static const size_t kWaiters = 4;

    wg.Add(kWorkers);

    for (size_t i = 0; i < kWaiters; ++i) {
      fibers::Go(scheduler, [&] {
        wg.Wait();
        ASSERT_EQ(work.load(), kWorkers);
        ++acks;
      });
    }

    for (size_t i = 0; i < kWorkers; ++i) {
      fibers::Go(scheduler, [&] {
        std::this_thread::sleep_for(1s);
        ++work;
        wg.Done();
      });
    }

    scheduler.WaitIdle();

    ASSERT_EQ(acks.load(), kWaiters);

    scheduler.Stop();
  }

  SIMPLE_TEST(DoNotBlockThread) {
    executors::ThreadPool scheduler{1};
    scheduler.Start();

    fibers::WaitGroup wg;
    bool ok = false;

    fibers::Go(scheduler, [&] {
      wg.Wait();
      ok = true;
    });

    wg.Add(1);

    fibers::Go(scheduler, [&] {
      for (size_t i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(32ms);
        fibers::Yield();
      }
      wg.Done();
    });

    scheduler.WaitIdle();

    ASSERT_TRUE(ok);

    scheduler.Stop();
  }

  SIMPLE_TEST(DoNotWasteCpu) {
    executors::ThreadPool scheduler{/*threads=*/4};
    scheduler.Start();

    fibers::WaitGroup wg;

    std::atomic<size_t> workers = 0;

    wheels::ProcessCPUTimer cpu_timer;

    static const size_t kWorkers = 3;

    wg.Add(kWorkers);

    fibers::Go(scheduler, [&] {
      wg.Wait();
      ASSERT_EQ(workers.load(), kWorkers);
    });

    std::this_thread::sleep_for(1s);

    for (size_t i = 0; i < kWorkers; ++i) {
      fibers::Go(scheduler, [&] {
        ++workers;
        wg.Done();
      });
    }

    scheduler.WaitIdle();

    ASSERT_TRUE(cpu_timer.Spent() < 100ms);

    scheduler.Stop();
  }
}

RUN_ALL_TESTS()
