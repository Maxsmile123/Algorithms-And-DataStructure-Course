#include <exe/executors/thread_pool.hpp>
#include <exe/executors/submit.hpp>

#include <exe/threads/wait_group.hpp>

#include <course/twist/test.hpp>

#include <twist/test/repeat.hpp>
#include <twist/test/random.hpp>

#include <twist/ed/stdlike/thread.hpp>
#include <twist/ed/stdlike/chrono.hpp>

#include <fmt/core.h>
#include <fmt/std.h>

#include <chrono>

using namespace exe;
using namespace std::chrono_literals;

// Parking + Balancing is hard

auto SteadyNow() {
  return twist::ed::stdlike::steady_clock::now();
}

void ExternalSubmits() {
  static const size_t kThreads = 4;

  executors::ThreadPool pool{kThreads};
  pool.Start();

  twist::test::Repeat repeat;

  while (repeat()) {
    auto start = SteadyNow();

    size_t tasks = 1 + repeat.Iter() % kThreads;

    threads::WaitGroup wg;
    wg.Add(tasks);

    for (size_t i = 0; i < tasks; ++i) {
      executors::Submit(pool, [&] {
        twist::ed::stdlike::this_thread::sleep_for(1s);
        wg.Done();
      });
    }

    wg.Wait();

    auto elapsed = SteadyNow() - start;
    ASSERT_TRUE(elapsed < 1500ms);
  }

  fmt::println("Iterations: {}", repeat.IterCount());

  pool.Stop();
}

void InternalSubmits() {
  static const size_t kThreads = 4;

  executors::ThreadPool pool{kThreads};
  pool.Start();

  twist::test::Repeat repeat;

  while (repeat()) {
    auto start = SteadyNow();

    threads::WaitGroup wg;
    wg.Add(1);

    size_t tasks = 1 + repeat.IterCount() % kThreads;

    executors::Submit(pool, [&] {
      wg.Add(tasks);

      for (size_t i = 0; i < tasks; ++i) {
        executors::Submit(pool, [&] {
          twist::ed::stdlike::this_thread::sleep_for(1s);
          wg.Done();
        });
      }

      wg.Done();
    });

    wg.Wait();

    auto elapsed = SteadyNow() - start;

    ASSERT_TRUE(elapsed < 1500ms);
  }

  fmt::println("Iterations: {}", repeat.IterCount());

  pool.Stop();
}

void MixedSubmits() {
  executors::ThreadPool pool{6};
  pool.Start();

  twist::test::Repeat repeat;

  while (repeat()) {
    auto start = SteadyNow();

    size_t ext_tasks = 1 + repeat.Iter() % 3;

    threads::WaitGroup wg;

    auto job = [&wg] {
      twist::ed::stdlike::this_thread::sleep_for(1s);
      wg.Done();
    };

    wg.Add(ext_tasks);

    for (size_t i = 0; i < ext_tasks; ++i) {
      bool spawn = twist::test::Random2();

      if (spawn) {
        wg.Add(1);
      }

      executors::Submit(pool, [&, spawn] {
        if (spawn) {
          executors::Submit(pool, [&] {
            job();
          });
        }

        job();
      });
    }

    wg.Wait();

    auto elapsed = SteadyNow() - start;

    ASSERT_TRUE(elapsed < 1500ms);
  }

  fmt::println("Iterations: {}", repeat.IterCount());

  pool.Stop();
}

TEST_SUITE(BalanceTasks) {
  TWIST_TEST(ExternalSubmits, 5s) {
    ExternalSubmits();
  }

  TWIST_TEST(InternalSubmits, 5s) {
    InternalSubmits();
  }

  TWIST_TEST(MixedSubmits, 5s) {
    MixedSubmits();
  }
}

RUN_ALL_TESTS()
