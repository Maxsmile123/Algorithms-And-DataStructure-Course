#include <exe/executors/manual.hpp>
#include <exe/executors/thread_pool.hpp>

#include <exe/gorr/core/task.hpp>
#include <exe/gorr/run/fire.hpp>
#include <exe/gorr/run/teleport.hpp>
#include <exe/gorr/run/yield.hpp>

#include <exe/gorr/sync/mutex.hpp>

#include <wheels/test/framework.hpp>

#include <iostream>

using namespace exe;

TEST_SUITE(TaskMutex) {
  SIMPLE_TEST(Yield) {
    executors::ManualExecutor scheduler;

    gorr::Mutex mutex;
    size_t counter = 0;

    static const size_t kSections = 3;
    static const size_t kContenders = 4;
    static const size_t kYields = 5;

    auto contender = [&]() -> gorr::Task<> {
      co_await gorr::TeleportTo(scheduler);

      for (size_t i = 0; i < kSections; ++i) {
        auto lock = co_await mutex.ScopedLock();

        ASSERT_TRUE(counter++ == 0);

        for (size_t k = 0; k < kYields; ++k) {
          co_await gorr::Yield(scheduler);
        }

        --counter;
      }
    };

    for (size_t j = 0; j < kContenders; ++j) {
      gorr::FireAndForget(contender());
    }

    size_t steps = scheduler.Drain();

    ASSERT_GE(steps, kContenders * kSections * kYields);
  }

  SIMPLE_TEST(ThreadPool) {
    executors::ThreadPool scheduler{4};

    gorr::Mutex mutex;
    size_t cs = 0;

    static const size_t kSections = 123456;
    static const size_t kContenders = 17;

    auto contender = [&]() -> gorr::Task<> {
      co_await gorr::TeleportTo(scheduler);

      for (size_t i = 0; i < kSections; ++i) {
        auto lock = co_await mutex.ScopedLock();
        ++cs;
      }
    };

    for (size_t j = 0; j < kContenders; ++j) {
      gorr::FireAndForget(contender());
    }

    scheduler.WaitIdle();

    ASSERT_EQ(cs, kContenders * kSections);

    scheduler.Stop();
  }
}

RUN_ALL_TESTS()
