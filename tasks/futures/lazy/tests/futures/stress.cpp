#include <wheels/test/framework.hpp>

#include <course/twist/test.hpp>
#include <twist/test/repeat.hpp>
#include <twist/test/race.hpp>

#include <exe/executors/thread_pool.hpp>

#include <exe/futures/make/contract.hpp>
#include <exe/futures/make/submit.hpp>

#include <exe/futures/combine/seq/map.hpp>
#include <exe/futures/combine/seq/flat_map.hpp>
#include <exe/futures/combine/seq/and_then.hpp>
#include <exe/futures/combine/seq/or_else.hpp>
#include <exe/futures/combine/seq/via.hpp>
#include <exe/futures/combine/seq/box.hpp>

#include <exe/futures/combine/par/all.hpp>
#include <exe/futures/combine/par/first.hpp>

#include <exe/futures/run/get.hpp>
#include <exe/futures/run/detach.hpp>

#include <exe/result/make/ok.hpp>
#include <exe/result/make/err.hpp>

#include <twist/ed/stdlike/thread.hpp>

#include <wheels/core/defer.hpp>

#include <fmt/core.h>

#include <atomic>
#include <chrono>

using namespace exe;
using namespace std::chrono_literals;

//////////////////////////////////////////////////////////////////////

std::error_code TimeoutError() {
  return std::make_error_code(std::errc::timed_out);
}

//////////////////////////////////////////////////////////////////////

void StressTestContract() {
  twist::test::Repeat repeat;

  while (repeat()) {
    auto [f, p] = futures::Contract<int>();

    int iter = repeat.Iter();

    twist::ed::stdlike::thread producer([p = std::move(p), iter]() mutable {
      std::move(p).SetValue(iter);
    });

    twist::ed::stdlike::thread consumer([f = std::move(f), iter]() mutable {
      auto r = std::move(f) | futures::Get();
      ASSERT_TRUE(r);
      ASSERT_EQ(*r, iter);
    });

    producer.join();
    consumer.join();
  }

  fmt::println("Iterations: {}", repeat.IterCount());
}

//////////////////////////////////////////////////////////////////////

void StressTestPipeline() {
  executors::ThreadPool pool{4};

  pool.Start();

  twist::test::Repeat repeat;

  while (repeat()) {
    size_t pipelines = 1 + repeat.Iter() % 3;

    std::atomic<size_t> counter1{0};
    std::atomic<size_t> counter2{0};
    std::atomic<size_t> counter3{0};

    std::vector<futures::BoxedFuture<Unit>> futs;

    for (size_t j = 0; j < pipelines; ++j) {
      auto f = futures::Submit(pool,
                      [&]() {
                        ++counter1;
                        return result::Ok();
                      })
          | futures::Via(pool)
          | futures::Map([&](Unit) -> Unit {
              ++counter2;
              return {};
            })
          | futures::Map([&](Unit) -> Unit {
              ++counter3;
              return {};
            });

      futs.push_back(std::move(f));
    }

    for (auto&& f : futs) {
      std::move(f) | futures::Get();
    }

    ASSERT_EQ(counter1.load(), pipelines);
    ASSERT_EQ(counter2.load(), pipelines);
    ASSERT_EQ(counter3.load(), pipelines);
  }

  fmt::println("Iterations: {}", repeat.IterCount());

  pool.Stop();
}

//////////////////////////////////////////////////////////////////////

void StressTestFirst() {
  executors::ThreadPool pool{4};
  pool.Start();

  twist::test::Repeat repeat;

  while (repeat()) {
    size_t i = repeat.Iter();

    std::atomic<size_t> done{0};

    auto f = futures::Submit(pool, [&, i]() -> Result<int> {
      wheels::Defer defer([&done] {
        ++done;
      });

      if (i % 3 == 0) {
        return result::Err(TimeoutError());
      } else {
        return result::Ok(1);
      }
    });

    auto g = futures::Submit(pool, [&, i]() -> Result<int> {
      wheels::Defer defer([&done] {
        ++done;
      });

      if (i % 4 == 0) {
        return result::Err(TimeoutError());
      } else {
        return result::Ok(2);
      }
    });

    auto first = futures::First(std::move(f), std::move(g));

    auto r = std::move(first) | futures::Get();

    if (i % 12 != 0) {
      ASSERT_TRUE(r);
      ASSERT_TRUE((*r == 1) || (*r == 2));
    } else {
      ASSERT_FALSE(r);
    }

    // Barrier
    while (done.load() != 2) {
      twist::ed::stdlike::this_thread::yield();
    }
  }

  fmt::println("Iterations: {}", repeat.IterCount());

  pool.Stop();
}

//////////////////////////////////////////////////////////////////////

void StressTestAll() {
  executors::ThreadPool pool{4};
  pool.Start();

  twist::test::Repeat repeat;

  while (repeat()) {
    size_t i = repeat.Iter();

    auto f = futures::Submit(pool, [i]() -> Result<int> {
      if (i % 7 == 5) {
        return result::Err(TimeoutError());
      } else {
        return result::Ok(1);
      }
    });

    auto g = futures::Submit(pool, [i]() -> Result<int> {
      if (i % 7 == 6) {
        return result::Err(TimeoutError());
      } else {
        return result::Ok(2);
      }
    });

    auto both = futures::Both(std::move(f), std::move(g));

    auto r = std::move(both) | futures::Get();

    if (i % 7 < 5) {
      auto [x, y] = *r;
      ASSERT_EQ(x, 1);
      ASSERT_EQ(y, 2);
    } else {
      ASSERT_FALSE(r);
    }
  }

  fmt::println("Iterations: {}", repeat.IterCount());

  pool.Stop();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(Futures) {
  TWIST_TEST(StressContract, 5s) {
    StressTestContract();
  }

  TWIST_TEST(StressPipeline, 5s) {
    StressTestPipeline();
  }

  TWIST_TEST(StressFirst, 5s) {
    StressTestFirst();
  }

  TWIST_TEST(StressBoth, 5s) {
    StressTestAll();
  }
}

RUN_ALL_TESTS()
