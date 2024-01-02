#include <wheels/test/framework.hpp>

#include <exe/executors/manual.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sync/event.hpp>

#include <atomic>

using namespace exe;

TEST_SUITE(Event) {
  SIMPLE_TEST(OneWaiter) {
    executors::ManualExecutor scheduler;

    static const std::string kMessage = "Hello";

    fibers::Event event;
    std::string data;
    bool ok = false;

    fibers::Go(scheduler, [&] {
      event.Wait();
      ASSERT_EQ(data, kMessage);
      ok = true;
    });

    fibers::Go(scheduler, [&] {
      data = kMessage;
      event.Fire();
    });

    scheduler.Drain();

    ASSERT_TRUE(ok);
  }

  SIMPLE_TEST(MultipleWaiters) {
    executors::ManualExecutor scheduler;

    fibers::Event event;
    int work = 0;
    size_t waiters = 0;

    static const size_t kWaiters = 7;

    for (size_t i = 0; i < kWaiters; ++i) {
      fibers::Go(scheduler, [&] {
        event.Wait();
        ASSERT_EQ(work, 1);
        ++waiters;
      });
    }

    fibers::Go(scheduler, [&] {
      ++work;
      event.Fire();
    });

    scheduler.Drain();

    ASSERT_EQ(waiters, kWaiters);
  }

  SIMPLE_TEST(SuspendFiber) {
    executors::ManualExecutor scheduler;

    fibers::Event event;
    bool ok = false;

    fibers::Go(scheduler, [&] {
      event.Wait();
      ok = true;
    });

    {
      size_t tasks = scheduler.Drain();
      ASSERT_LE(tasks, 7);
    }

    fibers::Go(scheduler, [&] {
      event.Fire();
    });

    scheduler.Drain();

    ASSERT_TRUE(ok);
  }
}

RUN_ALL_TESTS()
