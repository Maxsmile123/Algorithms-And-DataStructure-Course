#include <course/twist/test.hpp>

#include <exe/executors/thread_pool.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sched/yield.hpp>
#include <exe/fibers/sync/event.hpp>

#include <exe/threads/wait_group.hpp>

#include <twist/test/repeat.hpp>

using namespace exe;

//////////////////////////////////////////////////////////////////////

void StressTest() {
  executors::ThreadPool scheduler{5};
  scheduler.Start();

  for (twist::test::Repeat repeat; repeat(); ) {
    const size_t waiters = 1 + repeat.Iter() % 4;

    threads::WaitGroup iter;
    iter.Add(waiters);

    fibers::Event event;
    bool work = false;

    for (size_t i = 0; i < waiters; ++i) {
      fibers::Go(scheduler, [&] {
        event.Wait();
        ASSERT_TRUE(work);
        iter.Done();
      });
    }

    fibers::Go(scheduler, [&] {
      work = true;
      event.Fire();
    });

    iter.Wait();
  }

  scheduler.Stop();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(Event) {
  TWIST_TEST(Stress, 5s) {
    StressTest();
  }
}

RUN_ALL_TESTS();
