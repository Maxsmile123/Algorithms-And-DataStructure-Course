#include <wheels/test/framework.hpp>

#include <exe/executors/manual.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sync/wait_group.hpp>

#include <atomic>
#include <thread>

using namespace exe;

TEST_SUITE(WaitGroup) {
  SIMPLE_TEST(OneWaiter) {
    executors::ManualExecutor scheduler;

    fibers::WaitGroup wg;
    size_t work = 0;
    bool ok = false;

    static const size_t kWorkers = 3;

    wg.Add(kWorkers);

    fibers::Go(scheduler, [&] {
      wg.Wait();
      ASSERT_EQ(work, kWorkers);
      ok = true;
    });

    for (size_t i = 0; i < kWorkers; ++i) {
      fibers::Go(scheduler, [&] {
        ++work;
        wg.Done();
      });
    }

    scheduler.Drain();

    ASSERT_TRUE(ok);
  }

  SIMPLE_TEST(MultipleWaiters) {
    executors::ManualExecutor scheduler;

    fibers::WaitGroup wg;

    size_t work = 0;
    size_t acks = 0;

    static const size_t kWorkers = 3;
    static const size_t kWaiters = 4;

    wg.Add(kWorkers);

    for (size_t i = 0; i < kWaiters; ++i) {
      fibers::Go(scheduler, [&] {
        wg.Wait();
        ASSERT_EQ(work, kWorkers);
        ++acks;
      });
    }

    for (size_t i = 0; i < kWorkers; ++i) {
      fibers::Go(scheduler, [&] {
        ++work;
        wg.Done();
      });
    }

    scheduler.Drain();

    ASSERT_EQ(acks, kWaiters);
  }

  SIMPLE_TEST(SuspendFiber) {
    executors::ManualExecutor scheduler;

    fibers::WaitGroup wg;
    size_t work = 0;
    bool ok = false;

    static const size_t kWorkers = 3;

    wg.Add(kWorkers);

    fibers::Go(scheduler, [&] {
      wg.Wait();
      ASSERT_EQ(work, kWorkers);
      ok = true;
    });

    {
      size_t tasks = scheduler.Drain();
      ASSERT_LE(tasks, 7);
    }

    for (size_t i = 0; i < kWorkers; ++i) {
      fibers::Go(scheduler, [&] {
        ++work;
        wg.Done();
      });
    }

    scheduler.Drain();

    ASSERT_TRUE(ok);
  }
}

RUN_ALL_TESTS()
