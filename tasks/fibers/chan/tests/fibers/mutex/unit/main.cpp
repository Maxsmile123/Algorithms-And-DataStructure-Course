#include <exe/executors/thread_pool.hpp>

#include <exe/fibers/core/api.hpp>
#include <exe/fibers/sync/mutex.hpp>

#include <wheels/test/framework.hpp>

#include <wheels/support/cpu_time.hpp>

#include <atomic>
#include <chrono>
#include <thread>

using namespace exe;

using namespace std::chrono_literals;

TEST_SUITE(Mutex) {
  SIMPLE_TEST(JustWorks) {
    executors::ThreadPool scheduler{4};

    fibers::Mutex mutex;
    size_t cs = 0;

    fibers::Go(scheduler, [&]() {
      for (size_t j = 0; j < 11; ++j) {
        std::lock_guard guard(mutex);
        ++cs;
      }
    });

    scheduler.WaitIdle();

    ASSERT_EQ(cs, 11);

    scheduler.Stop();
  }

  SIMPLE_TEST(Counter) {
    executors::ThreadPool scheduler{4};

    fibers::Mutex mutex;
    size_t cs = 0;

    for (size_t i = 0; i < 10; ++i) {
      fibers::Go(scheduler, [&]() {
        for (size_t j = 0; j < 1024; ++j) {
          std::lock_guard guard(mutex);
          ++cs;
        }
      });
    }

    scheduler.WaitIdle();

    ASSERT_EQ(cs, 1024 * 10);

    scheduler.Stop();
  }

  SIMPLE_TEST(Blocking) {
    executors::ThreadPool scheduler{4};

    fibers::Mutex mutex;

    wheels::ProcessCPUTimer timer;

    fibers::Go(scheduler, [&]() {
      mutex.Lock();
      std::this_thread::sleep_for(1s);
      mutex.Unlock();
    });

    fibers::Go(scheduler, [&]() {
      mutex.Lock();
      mutex.Unlock();
    });

    scheduler.WaitIdle();

    ASSERT_TRUE(timer.Elapsed() < 100ms);

    scheduler.Stop();
  }
}

RUN_ALL_TESTS()
