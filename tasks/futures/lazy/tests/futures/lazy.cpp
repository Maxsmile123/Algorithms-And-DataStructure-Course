#include <exe/executors/thread_pool.hpp>
#include <exe/executors/manual.hpp>

#include <exe/futures/make/contract.hpp>
#include <exe/futures/make/value.hpp>
#include <exe/futures/make/failure.hpp>
#include <exe/futures/make/submit.hpp>
#include <exe/futures/make/just.hpp>

#include <exe/futures/combine/seq/map.hpp>
#include <exe/futures/combine/seq/and_then.hpp>
#include <exe/futures/combine/seq/or_else.hpp>
#include <exe/futures/combine/seq/flatten.hpp>
#include <exe/futures/combine/seq/flat_map.hpp>
#include <exe/futures/combine/seq/via.hpp>
#include <exe/futures/combine/seq/box.hpp>
#include <exe/futures/combine/seq/start.hpp>

#include <exe/futures/combine/par/first.hpp>
#include <exe/futures/combine/par/all.hpp>

#include <exe/futures/run/get.hpp>
#include <exe/futures/run/detach.hpp>

#include <exe/result/make/ok.hpp>
#include <exe/result/make/err.hpp>

#include <wheels/test/framework.hpp>

#include <thread>
#include <chrono>

using namespace exe;

using namespace std::chrono_literals;

std::error_code TimeoutError() {
  return std::make_error_code(std::errc::timed_out);
}

std::error_code IoError() {
  return std::make_error_code(std::errc::io_error);
}

// clang-format off

TEST_SUITE(LazyFutures) {
  SIMPLE_TEST(JustMap) {
    bool run = false;

    auto f = futures::Just() | futures::Map([&](Unit) {
      run = true;
      return Unit{};
    });

    ASSERT_FALSE(run);

    auto r = std::move(f) | futures::Get();
    ASSERT_TRUE(r);
  }

  SIMPLE_TEST(ContractMap) {
    auto [f, p] = futures::Contract<Unit>();

    bool run = false;

    auto g = std::move(f)
             | futures::Map([&](Unit) {
                 run = true;
                 return Unit{};
               });

    std::move(p).SetValue(Unit{});

    ASSERT_FALSE(run);

    std::move(g) | futures::Detach();

    ASSERT_TRUE(run);
  }

  SIMPLE_TEST(Submit) {
    executors::ManualExecutor manual;

    auto f = futures::Submit(manual, [] {
               return result::Ok(7);
             });

    ASSERT_TRUE(manual.IsEmpty());

    std::move(f) | futures::Detach();

    ASSERT_EQ(manual.Drain(), 1);
  }

  SIMPLE_TEST(Monad) {
    executors::ManualExecutor manual;

    auto f = futures::Just()
             | futures::Via(manual)
             | futures::AndThen([](Unit) {
                 return result::Ok(1);
               })
             | futures::Map([](int v) {
                 return v + 1;
               })
             | futures::OrElse([](Error) {
                 return result::Ok(13);
               });

    ASSERT_TRUE(manual.IsEmpty());

    std::move(f) | futures::Detach();

    // ASSERT_EQ
    ASSERT_GE(manual.Drain(), 2);
  }

  SIMPLE_TEST(Flatten) {
    executors::ManualExecutor manual;

    auto f = futures::Value(1)
              | futures::Map([&manual](int v) {
                  return futures::Submit(manual, [v] {
                    return result::Ok(v + 1);
                  });
                })
              | futures::Flatten();

    ASSERT_TRUE(manual.IsEmpty());

    std::move(f) | futures::Detach();

    ASSERT_TRUE(manual.Drain() > 0);
  }

  SIMPLE_TEST(FlatMap) {
    executors::ManualExecutor manual;

    auto f = futures::Just()
             | futures::FlatMap([&manual](Unit) {
                 return futures::Submit(manual, [] {
                   return result::Ok(11);
                 });
               });

    ASSERT_TRUE(manual.IsEmpty());

    std::move(f) | futures::Detach();

    ASSERT_TRUE(manual.Drain() > 0);
  }

  SIMPLE_TEST(Box) {
    executors::ManualExecutor manual;

    futures::BoxedFuture<int> f = futures::Just()
                                  | futures::Via(manual)
                                  | futures::Map([](Unit) {
                                      return 7;
                                    })
                                  | futures::Box();

    ASSERT_TRUE(manual.IsEmpty());

    std::move(f) | futures::Detach();

    ASSERT_TRUE(manual.Drain() > 0);
  }

  SIMPLE_TEST(First) {
    executors::ManualExecutor manual;

    auto f = futures::Just()
             | futures::Via(manual)
             | futures::Map([](Unit) {
                 return 1;
               });

    auto g = futures::Just()
             | futures::Via(manual)
             | futures::Map([](Unit) {
                 return 2;
               });

    auto first = futures::First(std::move(f), std::move(g));

    ASSERT_TRUE(manual.IsEmpty());

    std::move(first) | futures::Detach();

    ASSERT_EQ(manual.Drain(), 2);
  }

  SIMPLE_TEST(Start) {
    executors::ManualExecutor manual;

    auto f = futures::Just()
             | futures::Via(manual)
             | futures::Map([&](Unit) {
                 return 7;
               })
             | futures::Start();

    ASSERT_TRUE(manual.NonEmpty());
    manual.Drain();

    auto r = std::move(f) | futures::Get();

    ASSERT_TRUE(r);
    ASSERT_EQ(*r, 7);
  }

  SIMPLE_TEST(StartMap) {
    executors::ManualExecutor manual;

    auto f = futures::Just()
             | futures::Via(manual)
             | futures::Map([&](Unit) {
                 return 7;
               })
             | futures::Start()
             | futures::Map([](int v) {
                 return v + 1;
               });

    {
      size_t tasks = manual.Drain();
      ASSERT_EQ(tasks, 1);
    }

    std::move(f) | futures::Detach();

    {
      size_t tasks = manual.Drain();
      ASSERT_EQ(tasks, 1);
    }
  }
}

RUN_ALL_TESTS()
