#include <course/twist/test.hpp>

#include <exe/executors/thread_pool.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sched/yield.hpp>

#include <exe/fibers/sync/event.hpp>

#include <twist/test/repeat.hpp>

using namespace exe;

//////////////////////////////////////////////////////////////////////

void StressTest() {
  executors::ThreadPool scheduler{5};
  scheduler.Start();

  for (twist::test::Repeat repeat; repeat.Test(); ) {
    size_t waiters = 1 + repeat.Iter() % 4;

    fibers::Event event;
    bool work = false;
    std::atomic<size_t> acks{0};

    for (size_t i = 0; i < waiters; ++i) {
      fibers::Go(scheduler, [&] {
        event.Wait();
        ASSERT_TRUE(work);
        ++acks;
      });
    }

    fibers::Go(scheduler, [&] {
      work = true;
      event.Fire();
    });

    scheduler.WaitIdle();

    ASSERT_EQ(acks.load(), waiters);
  }

  scheduler.Stop();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(Event) {
  TWIST_TEST(Event, 5s) {
    StressTest();
  }
}

RUN_ALL_TESTS();
