#include <course/twist/test.hpp>

#include <exe/executors/thread_pool.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sync/event.hpp>

#include <twist/test/repeat.hpp>

using namespace exe;

//////////////////////////////////////////////////////////////////////

void StorageTest() {
  executors::ThreadPool scheduler{5};
  scheduler.Start();

  for (twist::test::Repeat repeat; repeat.Test(); ) {
    fibers::Go(scheduler, [] {
      auto* event = new fibers::Event{};

      fibers::Go([event] {
        event->Fire();
      });

      event->Wait();
      delete event;
    });

    scheduler.WaitIdle();
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
