#include <course/twist/test.hpp>

#include <exe/executors/thread_pool.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sync/event.hpp>
#include <exe/threads/wait_group.hpp>

#include <twist/test/repeat.hpp>

using namespace exe;

//////////////////////////////////////////////////////////////////////

void StorageTest() {
  executors::ThreadPool scheduler{5};
  scheduler.Start();

  for (twist::test::Repeat repeat; repeat(); ) {
    threads::WaitGroup iter;
    iter.Add(1);

    fibers::Go(scheduler, [&iter] {
      auto* event = new fibers::Event{};

      fibers::Go([event] {
        event->Fire();
      });

      event->Wait();
      delete event;

      iter.Done();
    });

    iter.Wait();
  }

  scheduler.Stop();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(Event) {
  TWIST_TEST(Storage, 5s) {
    StorageTest();
  }
}

RUN_ALL_TESTS();
