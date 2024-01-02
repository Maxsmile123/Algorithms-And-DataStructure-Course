#include <wheels/test/framework.hpp>
#include <twist/test/budget.hpp>

#include <course/twist/test.hpp>
#include <twist/test/plate.hpp>

#include <exe/executors/thread_pool.hpp>

#include <exe/gorr/run/fire.hpp>
#include <exe/gorr/run/teleport.hpp>
#include <exe/gorr/run/yield.hpp>

#include <exe/gorr/sync/mutex.hpp>

#include <atomic>
#include <chrono>

using namespace exe;
using namespace std::chrono_literals;

//////////////////////////////////////////////////////////////////////

void StressTest1(size_t fibers) {
  executors::ThreadPool scheduler{4};

  gorr::Mutex mutex;
  twist::test::Plate plate;

  auto contender = [&]() -> gorr::Task<> {
    co_await gorr::TeleportTo(scheduler);

    size_t iter = 0;
    while (twist::test::KeepRunning()) {
      auto lock = co_await mutex.ScopedLock();

      plate.Access();
      if (++iter % 7 == 0) {
        co_await gorr::Yield(scheduler);
      }
    }
  };

  for (size_t i = 0; i < fibers; ++i) {
    gorr::FireAndForget(contender());
  }

  scheduler.WaitIdle();

  std::cout << "# critical sections: " << plate.AccessCount() << std::endl;
  ASSERT_TRUE(plate.AccessCount() > 100500);

  scheduler.Stop();
}

//////////////////////////////////////////////////////////////////////

void StressTest2(size_t gorroutines) {
  executors::ThreadPool scheduler{4};

  while (twist::test::KeepRunning()) {
    gorr::Mutex mutex;
    size_t cs = 0;

    auto contender = [&]() -> gorr::Task<> {
      co_await gorr::TeleportTo(scheduler);

      {
        auto lock = co_await mutex.ScopedLock();
        ++cs;
      }
    };

    for (size_t j = 0; j < gorroutines; ++j) {
      gorr::FireAndForget(contender());
    }

    scheduler.WaitIdle();

    ASSERT_EQ(cs, gorroutines);
  }

  scheduler.Stop();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(GorrMutex) {
  TWIST_TEST(Stress_1_1, 10s) {
    StressTest1(/*fibers=*/4);
  }

  TWIST_TEST(Stress_1_2, 5s) {
    StressTest1(/*fibers=*/16);
  }

  TWIST_TEST(Stress_1_3, 5s) {
    StressTest1(/*fibers=*/100);
  }

  TWIST_TEST(Stress_2_1, 10s) {
    StressTest2(/*fibers=*/2);
  }

  TWIST_TEST(Stress_2_2, 10s) {
    StressTest2(/*fibers=*/3);
  }
}

RUN_ALL_TESTS()
