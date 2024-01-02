#include <course/twist/test.hpp>

#include <twist/test/plate.hpp>
#include <twist/test/repeat.hpp>

#include <exe/executors/thread_pool.hpp>
#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sync/mutex.hpp>

#include <atomic>
#include <chrono>

using namespace exe;
using namespace std::chrono_literals;

//////////////////////////////////////////////////////////////////////

void StressTest1(size_t fibers) {
  executors::ThreadPool scheduler{4};
  scheduler.Start();

  fibers::Mutex mutex;
  twist::test::Plate plate;

  for (size_t i = 0; i < fibers; ++i) {
    fibers::Go(scheduler, [&] {
      for (twist::test::TimeBudget budget; budget.Withdraw(); ) {
        mutex.Lock();
        plate.Access();
        mutex.Unlock();
      }
    });
  }

  scheduler.WaitIdle();

  std::cout << "# critical sections: " << plate.AccessCount() << std::endl;

  scheduler.Stop();
}

//////////////////////////////////////////////////////////////////////

void StressTest2() {
  executors::ThreadPool scheduler{4};
  scheduler.Start();

  for (twist::test::Repeat repeat; repeat.Test(); ) {
    size_t fibers = 2 + repeat.Iter() % 5;

    fibers::Mutex mutex;
    std::atomic<size_t> cs{0};

    for (size_t j = 0; j < fibers; ++j) {
      fibers::Go(scheduler, [&] {
        mutex.Lock();
        ++cs;
        mutex.Unlock();
      });
    }

    scheduler.WaitIdle();

    ASSERT_EQ(cs, fibers);
  }

  scheduler.Stop();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(Mutex) {
  TWIST_TEST(Stress_1_4, 5s) {
    StressTest1(/*fibers=*/4);
  }

  TWIST_TEST(Stress_1_16, 5s) {
    StressTest1(/*fibers=*/16);
  }

  TWIST_TEST(Stress_2, 5s) {
    StressTest2();
  }
}

RUN_ALL_TESTS()
