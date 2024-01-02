#include <exe/executors/thread_pool.hpp>
#include <exe/executors/submit.hpp>

#include <exe/futures/make/contract.hpp>
#include <exe/futures/make/submit.hpp>
#include <exe/futures/make/value.hpp>
#include <exe/futures/make/failure.hpp>
#include <exe/futures/make/just.hpp>

#include <exe/futures/combine/seq/map.hpp>
#include <exe/futures/combine/seq/and_then.hpp>
#include <exe/futures/combine/seq/or_else.hpp>
#include <exe/futures/combine/seq/via.hpp>
#include <exe/futures/combine/seq/flatten.hpp>
#include <exe/futures/combine/seq/flat_map.hpp>

#include <exe/futures/combine/par/all.hpp>
#include <exe/futures/combine/par/first.hpp>

#include <exe/futures/terminate/get.hpp>
#include <exe/futures/terminate/detach.hpp>

#include <exe/futures/syntax/both.hpp>
#include <exe/futures/syntax/or.hpp>

#include <exe/result/make/ok.hpp>
#include <exe/result/make/err.hpp>

#include <wheels/core/panic.hpp>

#include <fmt/core.h>
#include <fmt/std.h>

using namespace exe;

Error TimeoutError() {
  return std::make_error_code(std::errc::timed_out);
}

int main() {
  executors::ThreadPool pool{4};
  pool.Start();

  {
    // Contract

    auto [f, p] = futures::Contract<int>();

    {
      // Producer
      executors::Submit(pool, [p = std::move(p)]() mutable {
        std::move(p).SetValue(5);
      });
    }

    {
      // Consumer
      auto r = std::move(f) | futures::Get();

      fmt::println("Contract -> {}", *r);
    }
  }

  {
    // Submit

    auto f = futures::Submit(pool,
                             []() {
                               fmt::println("Running on thread pool");
                               return result::Ok(7);
                             });

    auto r = std::move(f) | futures::Get();

    fmt::println("Submit -> {}", *r);
  }

  {
    // Value

    auto f = futures::Value(42);

    auto r = std::move(f) | futures::Get();

    fmt::println("Value -> {}", *r);
  }

  {
    // Failure

    auto r = futures::Failure<int>(/*with=*/TimeoutError()) | futures::Get();

    if (!r) {
      fmt::println("Failure -> {}", r.error());
    }
  }

  {
    // Map

    // Map: Future<T> -> (T -> U) -> Future<U>

    auto f = futures::Submit(pool,
                             []() {
                               fmt::println("Running on thread pool");
                               return result::Ok(1);
                             })
             | futures::Map([](int v) {
                 return v + 1;
               });

    auto r = std::move(f) | futures::Get();

    fmt::println("Submit.Map -> {}", r.value());
  }

  {
    // Detach

    futures::Submit(pool, [] {
      return result::Ok();
    }) | futures::Detach();
  }

  {
    auto f = futures::Value(1)
             | futures::Map([](int v) {
                 fmt::println("Inlined");
                 return v + 1;
               })
             | futures::Via(pool)
             | futures::Map([](int v) {
                 fmt::println("Running on thread pool");
                 return v + 1;
               });

    auto r = std::move(f) | futures::Get();

    fmt::println("Value.Map.Via.Map -> {}", *r);
  }

  {
    // AndThen / OrElse

    // AndThen: Future<T> -> (T -> Result<U>) -> Future<U>
    // OrElse:  Future<T> -> (Error -> Result<T>) -> Future<T>

    auto f = futures::Value(1)
             | futures::AndThen([](int) -> Result<int> {
                 return result::Err(TimeoutError());
               })
             | futures::AndThen([](int) -> Result<int> {
                 wheels::Panic("Skip");
                 return -1;
               })
             | futures::OrElse([](std::error_code) -> Result<int> {
                 return 42;  // Fallback
               });

    auto r = std::move(f) | futures::Get();

    fmt::println("AndThen.OrElse -> {}", *r);
  }

  {
    // Flatten

    // Future<Future<T>> -> Future<T>

    futures::Future<int> f =
        futures::Submit(pool,
                        [&] {
                          return result::Ok(futures::Submit(pool, [] {
                            return result::Ok(7);
                          }));
                        }) |
        futures::Flatten();

    auto r = std::move(f) | futures::Get();

    fmt::println("Flatten -> {}", *r);
  }

  {
    // FlatMap = Map + Flatten
    // FlatMap: Future<T> -> (T -> Future<U>) -> Future<U>

    futures::Future<int> f = futures::Submit(pool,
                                             [] {
                                               return result::Ok(1);
                                             }) |
                             futures::FlatMap([&pool](int v) {
                               return futures::Submit(pool, [v] {
                                 return result::Ok(v + 1);
                               });
                             });

    auto r = std::move(f) | futures::Get();

    fmt::println("FlatMap -> {}", *r);
  }

  {
    // First

    auto f = futures::Submit(pool, [] {
      return result::Ok(1);
    });

    auto g = futures::Submit(pool, [] {
      return result::Ok(2);
    });

    // ~ futures::First(std::move(f), std::move(g))
    auto r = (std::move(f) or std::move(g)) | futures::Get();

    fmt::println("First -> {}", *r);

    pool.WaitIdle();
  }

  {
    // Both

    auto f = futures::Submit(pool, [] {
      return result::Ok(1);
    });

    auto g = futures::Submit(pool, [] {
      return result::Ok(2);
    });

    // ~ futures::Both(std::move(f), std::move(g))
    auto r = (std::move(f) + std::move(g)) | futures::Get();
    auto [x, y] = r.value();

    fmt::println("Both -> ({}, {})", x, y);
  }

  pool.Stop();

  return 0;
}
