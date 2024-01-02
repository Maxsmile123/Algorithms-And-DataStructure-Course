#include <exe/executors/thread_pool.hpp>
#include <exe/executors/submit.hpp>

#include <twist/ed/stdlike/thread.hpp>

#include <course/twist/test.hpp>

#include <fmt/core.h>

#include <chrono>

using namespace exe;
using namespace std::chrono_literals;

// Assume TWIST_FIBERS=ON

void FutexTest() {
  auto* scheduler = twist::rt::fiber::Scheduler::Current();
  size_t before = scheduler->FutexCount();

  executors::ThreadPool pool{4};
  pool.Start();

  twist::ed::stdlike::this_thread::sleep_for(1s);

  for (size_t i = 1; i <= 6; ++i) {
    // Burst
    for (size_t j = 1; j <= i; ++j) {
      executors::Submit(pool, [&, j] {
        if (j % 2 == 1) {
          executors::Submit(pool, []{});
        }
      });
    }

    twist::ed::stdlike::this_thread::sleep_for(1s);
  }

  size_t after = scheduler->FutexCount();
  size_t futex_count = after - before;

  fmt::println("Futex count = {}", futex_count);

  ASSERT_TRUE(futex_count < 128);

  pool.Stop();
}

TEST_SUITE(Parking) {
  TWIST_TEST(Futex, 1s) {
    FutexTest();
  }
}

RUN_ALL_TESTS()
