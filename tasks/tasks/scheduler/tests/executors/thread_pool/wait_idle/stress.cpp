#include <exe/executors/thread_pool.hpp>
#include <exe/executors/submit.hpp>

#include <course/twist/test.hpp>

#include <twist/test/repeat.hpp>
#include <twist/test/random.hpp>

#include <fmt/core.h>

using namespace exe;

////////////////////////////////////////////////////////////////////////////////

namespace tests {

void Spawners() {
  executors::ThreadPool pool{4};
  pool.Start();

  twist::test::Repeat repeat;

  while (repeat()) {
    std::atomic<size_t> tasks{0};
    std::atomic<size_t> done{0};

    size_t init = twist::test::Random(1, 10);

    tasks += init;

    for (size_t i = 0; i < init; ++i) {
      size_t spawn;
      if (twist::test::Random2()) {
        spawn = twist::test::Random(1, 100);
      } else {
        spawn = twist::test::Random(3);
      }

      tasks += spawn;

      Submit(pool, [&, spawn] {
        // Spawn tasks
        for (size_t j = 0; j < spawn; ++j) {
          Submit(pool, [&done] {
            ++done;
          });
        }

        ++done;
      });
    }

    size_t after = twist::test::Random(5);

    tasks += after;

    for (size_t k = 0; k < after; ++k) {
      executors::Submit(pool, [&done] {
        ++done;
      });
    }

    pool.WaitIdle();

    ASSERT_EQ(tasks.load(), done.load());
  }

  fmt::println("Iterations: {}", repeat.IterCount());

  pool.Stop();
}

}  // namespace tests

////////////////////////////////////////////////////////////////////////////////

TEST_SUITE(WaitIdle) {
  TWIST_TEST(Spawners, 10s) {
    tests::Spawners();
  }
}

RUN_ALL_TESTS()
