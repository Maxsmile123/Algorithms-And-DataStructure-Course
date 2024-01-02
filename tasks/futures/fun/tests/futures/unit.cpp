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

#include <exe/futures/combine/par/first.hpp>
#include <exe/futures/combine/par/all.hpp>

#include <exe/futures/terminate/get.hpp>
#include <exe/futures/terminate/detach.hpp>

#include <exe/result/make/ok.hpp>
#include <exe/result/make/err.hpp>

#include <wheels/test/framework.hpp>
#include <wheels/test/util/cpu_timer.hpp>

#include <string>
#include <thread>
#include <tuple>
#include <chrono>

using namespace exe;

using namespace std::chrono_literals;

std::error_code TimeoutError() {
  return std::make_error_code(std::errc::timed_out);
}

std::error_code IoError() {
  return std::make_error_code(std::errc::io_error);
}

struct MoveOnly {
  MoveOnly() = default;
  MoveOnly(const MoveOnly&) = delete;
  MoveOnly(MoveOnly&&) {};
};

struct NonDefaultConstructible {
  NonDefaultConstructible(int) {};
};

TEST_SUITE(Futures) {
  SIMPLE_TEST(ContractValue) {
    auto [f, p] = futures::Contract<std::string>();

    std::move(p).SetValue("Hi");
    auto r = std::move(f) | futures::Get();

    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(*r, "Hi");
  }

  SIMPLE_TEST(ContractError) {
    auto [f, p] = futures::Contract<int>();

    auto timeout = TimeoutError();

    std::move(p).SetError(timeout);
    auto r = std::move(f) | futures::Get();

    ASSERT_TRUE(!r);
    ASSERT_EQ(r.error(), timeout);
  }

  SIMPLE_TEST(ContractDetach) {
    {
      auto [f, p] = futures::Contract<int>();

      std::move(f) | futures::Detach();
      std::move(p).SetValue(1);
    }

    {
      auto [f, p] = futures::Contract<int>();

      std::move(p).SetValue(1);
      std::move(f) | futures::Detach();
    }
  }

  SIMPLE_TEST(ContractMoveOnly) {
    auto [f, p] = futures::Contract<MoveOnly>();

    std::move(p).SetValue(MoveOnly{});
    auto r = std::move(f) | futures::Get();

    ASSERT_TRUE(r);
  }

  SIMPLE_TEST(ContractNonDefaultConstructible) {
    auto [f, p] = futures::Contract<NonDefaultConstructible>();

    std::move(p).SetValue({128});
    auto r = std::move(f) | futures::Get();

    ASSERT_TRUE(r);
  }

  SIMPLE_TEST(Value) {
    auto r = futures::Value(std::string("Hi")) | futures::Get();

    ASSERT_TRUE(r);
    ASSERT_EQ(*r, "Hi");
  }

  SIMPLE_TEST(Just) {
    auto r = futures::Just() | futures::Get();
    ASSERT_TRUE(r);
    ASSERT_EQ(*r, Unit{});
  }

  SIMPLE_TEST(Failure) {
    auto timeout = TimeoutError();
    auto r = futures::Failure<int>(timeout) | futures::Get();

    ASSERT_FALSE(r);
    ASSERT_EQ(r.error(), timeout);
  }

  SIMPLE_TEST(SubmitPool) {
    executors::ThreadPool pool{4};
    pool.Start();

    auto f = futures::Submit(pool, [] {
      return result::Ok(11);
    });

    auto r = std::move(f) | futures::Get();

    ASSERT_TRUE(r);
    ASSERT_EQ(*r, 11);

    pool.Stop();
  }

  SIMPLE_TEST(SubmitManual) {
    executors::ManualExecutor manual;

    bool done = false;

    auto f = futures::Submit(manual, [&] {
      done = true;
      return result::Ok();
    });

    std::move(f) | futures::Detach();

    ASSERT_FALSE(done);

    manual.Drain();

    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(SubmitPoolError) {
    executors::ThreadPool pool{4};
    pool.Start();

    auto f = futures::Submit(pool, []() -> Result<int> {
      return result::Err(IoError());
    });

    auto r = std::move(f) | futures::Get();

    ASSERT_TRUE(!r);
    ASSERT_EQ(r.error(), IoError());

    pool.Stop();
  }

  SIMPLE_TEST(SubmitPoolWait) {
    executors::ThreadPool pool{4};
    pool.Start();

    auto f = futures::Submit(pool, [] {
      std::this_thread::sleep_for(1s);
      return result::Ok(11);
    });

    wheels::ProcessCPUTimer timer;

    auto r = std::move(f) | futures::Get();

    ASSERT_TRUE(timer.Spent() < 100ms);

    ASSERT_TRUE(r);
    ASSERT_EQ(*r, 11);

    pool.Stop();
  }

  SIMPLE_TEST(MapValue) {
    auto f = futures::Value(1)
             | futures::Map([](int v) {
                 return v + 1;
               });

    auto r = std::move(f) | futures::Get();

    ASSERT_TRUE(r);
    ASSERT_EQ(*r, 2);
  }

  SIMPLE_TEST(MapError) {
    auto f = futures::Failure<int>(TimeoutError())
             | futures::Map([](int) {
                 FAIL_TEST("Skip this mapper");
                 return Unit{};
               });

    auto r = std::move(f) | futures::Get();

    ASSERT_FALSE(r);
    ASSERT_EQ(r.error(), TimeoutError());
  }

  SIMPLE_TEST(AndThen) {
    auto f = futures::Value<std::string>("ok")
             | futures::AndThen([](std::string s) {
                 return result::Ok(s + "!");
               })
             | futures::AndThen([](std::string s) {
                 return result::Ok(s + "!");
               });

    auto r = std::move(f) | futures::Get();

    ASSERT_TRUE(r);
    ASSERT_EQ(*r, "ok!!");
  }

  SIMPLE_TEST(OrElse) {
    auto f = futures::Failure<std::string>(IoError())
             | futures::OrElse([](Error e) {
                 ASSERT_EQ(e, IoError());
                 return result::Ok("fallback");
               });

    auto r = std::move(f) | futures::Get();

    ASSERT_TRUE(r);
    ASSERT_EQ(*r, "fallback");
  }

  SIMPLE_TEST(Pipeline) {
    auto [f, p] = futures::Contract<int>();

    auto g = std::move(f) | futures::Map([](int v) {
               return v + 1;
             }) | futures::Map([](int v) {
               return v + 2;
             }) | futures::OrElse([](Error) {
               FAIL_TEST("Skip this");
               return result::Ok(111);
             }) | futures::AndThen([](int) -> Result<int> {
               return result::Err(TimeoutError());
             }) | futures::AndThen([](int v) {
               FAIL_TEST("Skip this");
               return result::Ok(v + 3);
             }) | futures::Map([](int v) {
               FAIL_TEST("Skip this");
               return v + 4;
             }) | futures::OrElse([](Error) -> Result<int> {
               return 17;
             }) | futures::Map([](int v) {
               return v + 1;
             });

    std::move(p).Set(3);

    auto r = std::move(g) | futures::Get();

    ASSERT_TRUE(r);
    ASSERT_EQ(*r, 18);
  }

  SIMPLE_TEST(Flatten1) {
    executors::ManualExecutor manual;

    auto ff = futures::Submit(manual, [&manual]() {
      auto f = futures::Submit(manual, [] {
        return result::Ok(7);
      });
      return result::Ok(std::move(f));
    });

    futures::Future<int> f = std::move(ff) | futures::Flatten();

    bool ok = false;

    std::move(f) | futures::Map([&ok](int v) -> Unit {
      ASSERT_EQ(v, 7);
      ok = true;
      return {};
    }) | futures::Detach();

    ASSERT_FALSE(ok);

    manual.Drain();

    ASSERT_TRUE(ok);
  }

  SIMPLE_TEST(Flatten2) {
    auto ff = futures::Failure<futures::Future<int>>(IoError());

    Result<int> r = std::move(ff) | futures::Flatten() | futures::Get();

    ASSERT_FALSE(r);
  }

  SIMPLE_TEST(FlatMap) {
    executors::ManualExecutor manual;

    futures::Future<int> f = futures::Submit(manual, [] {
                               return result::Ok(23);
                             })
                             | futures::FlatMap([&](int v) {
                                 return futures::Submit(manual, [v] {
                                   return result::Ok(v + 5);
                                 });
                               })
                             | futures::Map([](int v) {
                                 return v + 1;
                               });

    bool ok = true;

    std::move(f)
        | futures::Map([&](int v) {
            ASSERT_EQ(v, 29);
            ok = true;
            return Unit{};
          })
        | futures::Detach();

    manual.Drain();

    ASSERT_TRUE(ok);
  }

  SIMPLE_TEST(Via) {
    executors::ManualExecutor manual1;
    executors::ManualExecutor manual2;

    size_t steps = 0;

    futures::Just()
        | futures::Via(manual1)
        | futures::Map([&](Unit) {
            ++steps;
            return Unit{};
          })
        | futures::AndThen([&](Unit) -> Status {
            ++steps;
            return result::Ok();
          })
        | futures::Map([](Unit) {
            return Unit{};
          })
        | futures::Via(manual2)
        | futures::Map([&](Unit) {
            ++steps;
            return Unit{};
          })
        | futures::OrElse([&](Error) -> Status {
            FAIL_TEST("Skip this");
            return result::Ok();
          })
        | futures::Map([&](Unit) {
            ++steps;
            return Unit{};
          })
        | futures::Via(manual1)
        | futures::Map([&](Unit) {
            ++steps;
            return Unit{};
          })
        | futures::FlatMap([&](Unit) {
            ++steps;
            return futures::Value(1);
          })
        | futures::Map([&](int v) {
            ASSERT_EQ(v, 1);
            ++steps;
            return Unit{};
          })
        | futures::Detach();

    ASSERT_EQ(manual1.Drain(), 3);
    ASSERT_EQ(steps, 2);
    ASSERT_GE(manual2.Drain(), 2);
    ASSERT_EQ(steps, 4);
    ASSERT_GE(manual1.Drain(), 3);
    ASSERT_EQ(steps, 7);
  }

  SIMPLE_TEST(FirstOk1) {
    auto [f1, p1] = futures::Contract<int>();
    auto [f2, p2] = futures::Contract<int>();

    auto first = futures::First(std::move(f1), std::move(f2));

    bool ok = false;

    std::move(first)
        | futures::Map([&ok](int v) {
            ASSERT_EQ(v, 2);
            ok = true;
            return Unit{};
          })
        | futures::Detach();

    std::move(p2).SetValue(2);

    ASSERT_TRUE(ok);

    std::move(p1).SetValue(1);
  }

  SIMPLE_TEST(FirstOk2) {
    auto [f1, p1] = futures::Contract<int>();
    auto [f2, p2] = futures::Contract<int>();

    auto first = futures::First(std::move(f1), std::move(f2));

    bool ok = false;

    std::move(first)
        | futures::Map([&ok](int v) {
            ASSERT_EQ(v, 29);
            ok = true;
            return Unit{};
          })
        | futures::Detach();

    std::move(p1).SetError(TimeoutError());
    std::move(p2).SetValue(29);

    ASSERT_TRUE(ok);
  }

  SIMPLE_TEST(FirstOk3) {
    auto [f1, p1] = futures::Contract<int>();
    auto [f2, p2] = futures::Contract<int>();

    auto first = futures::First(std::move(f1), std::move(f2));

    bool ok = false;

    std::move(first)
        | futures::Map([&ok](int v) {
            ASSERT_EQ(v, 31);
            ok = true;
            return Unit{};
          })
        | futures::Detach();

    std::move(p2).SetError(IoError());
    std::move(p1).SetValue(31);

    ASSERT_TRUE(ok);
  }

  SIMPLE_TEST(FirstFailure) {
    auto [f1, p1] = futures::Contract<int>();
    auto [f2, p2] = futures::Contract<int>();

    auto first = futures::First(std::move(f1), std::move(f2));

    bool fail = false;

    std::move(first)
        | futures::OrElse([&](Error e) -> Result<int> {
            ASSERT_EQ(e, TimeoutError());
            fail = true;
            return result::Err(e);
          })
        | futures::Detach();

    std::move(p2).SetError(TimeoutError());
    std::move(p1).SetError(TimeoutError());

    ASSERT_TRUE(fail);
  }

  SIMPLE_TEST(BothOk) {
    auto [f1, p1] = futures::Contract<int>();
    auto [f2, p2] = futures::Contract<int>();

    auto both = futures::Both(std::move(f1), std::move(f2));

    bool ok = false;

    std::move(both)
        | futures::Map([&ok](auto tuple) {
            auto [x, y] = tuple;
            ASSERT_EQ(x, 2);
            ASSERT_EQ(y, 1);
            ok = true;
            return Unit{};
          })
        | futures::Detach();

    std::move(p2).SetValue(1);
    std::move(p1).SetValue(2);

    ASSERT_TRUE(ok);
  }

  SIMPLE_TEST(BothFailure1) {
    auto [f1, p1] = futures::Contract<int>();
    auto [f2, p2] = futures::Contract<int>();

    auto both = futures::Both(std::move(f1), std::move(f2));

    bool fail = false;

    std::move(both)
        | futures::OrElse([&fail](Error e) -> Result<std::tuple<int, int>> {
            ASSERT_EQ(e, TimeoutError());
            fail = true;
            return result::Err(e);
          })
        | futures::Detach();

    std::move(p1).SetError(TimeoutError());

    ASSERT_TRUE(fail);

    std::move(p2).SetValue(7);
  }

  SIMPLE_TEST(BothFailure2) {
    auto [f1, p1] = futures::Contract<int>();
    auto [f2, p2] = futures::Contract<int>();

    auto both = futures::Both(std::move(f1), std::move(f2));

    bool fail = false;

    std::move(both)
        | futures::OrElse([&fail](Error e) -> Result<std::tuple<int, int>> {
            ASSERT_EQ(e, IoError());
            fail = true;
            return result::Err(e);
          })
        | futures::Detach();

    std::move(p2).SetError(IoError());

    ASSERT_TRUE(fail);

    std::move(p1).SetValue(4);
  }

  SIMPLE_TEST(BothTypes) {
    auto [f1, p1] = futures::Contract<std::string>();
    auto [f2, p2] = futures::Contract<std::tuple<int, int>>();

    auto both = futures::Both(std::move(f1), std::move(f2));

    bool ok = false;

    std::move(both)
        | futures::Map([&ok](auto tuple) {
            auto [x, y] = tuple;

            ASSERT_EQ(x, "3");

            std::tuple<int, int> t = {1, 2};
            ASSERT_EQ(y, t);

            ok = true;
            return Unit{};
          })
        | futures::Detach();

    std::move(p2).SetValue({1, 2});
    std::move(p1).SetValue("3");

    ASSERT_TRUE(ok);
  }

  SIMPLE_TEST(DoNotWait1) {
    executors::ThreadPool pool{4};
    pool.Start();

    bool submit = false;

    auto f = futures::Submit(pool,
                             [&] {
                               std::this_thread::sleep_for(1s);
                               submit = true;
                               return result::Ok(11);
                             })
             | futures::Map([](int v) {
                 return v + 1;
               });

    ASSERT_FALSE(submit);

    auto r = std::move(f) | futures::Get();

    ASSERT_TRUE(submit);

    ASSERT_TRUE(r);
    ASSERT_EQ(*r, 12);

    pool.Stop();
  }

  SIMPLE_TEST(DoNotWait2) {
    executors::ThreadPool pool{4};
    pool.Start();

    bool submit = false;

    auto f = futures::Submit(pool,
                             [&] {
                               std::this_thread::sleep_for(1s);
                               submit = true;
                               return result::Ok(31);
                             })
             | futures::FlatMap([&](int v) {
                 return futures::Submit(pool, [v] {
                   return result::Ok(v + 1);
                 });
               });

    ASSERT_FALSE(submit);

    auto r = std::move(f) | futures::Get();

    ASSERT_TRUE(submit);

    ASSERT_TRUE(r);
    ASSERT_EQ(*r, 32);

    pool.Stop();
  }

  SIMPLE_TEST(Inline1) {
    executors::ManualExecutor manual;

    bool ok = false;

    futures::Just()
        | futures::Via(manual)
        | futures::Map([&](Unit) {
            ok = true;
            return Unit{};
          })
        | futures::Detach();

    size_t tasks = manual.Drain();
    ASSERT_TRUE(ok);
    ASSERT_EQ(tasks, 1);
  }

  SIMPLE_TEST(Inline2) {
    executors::ManualExecutor manual;

    auto r = futures::Value(1)
             | futures::Via(manual)
             | futures::Get();

    ASSERT_TRUE(r);
    ASSERT_EQ(*r, 1);
  }

  SIMPLE_TEST(Inline3) {
    executors::ManualExecutor manual;

    bool flat_map = false;
    bool map1 = false;
    bool map2 = false;

    futures::Just()
        | futures::Via(manual)
        | futures::FlatMap([&](Unit) {
            flat_map = true;
            return futures::Value(Unit{});
          })
        | futures::Map([&](Unit u) {
            map1 = true;
            return u;
          })
        | futures::Map([&](Unit u) {
            map2 = true;
            return u;
          })
        | futures::Detach();

    ASSERT_TRUE(manual.RunNext());
    ASSERT_TRUE(flat_map);
    ASSERT_FALSE(map1);

    ASSERT_TRUE(manual.RunNext());
    ASSERT_TRUE(map1);
    ASSERT_FALSE(map2);

    ASSERT_EQ(manual.Drain(), 1);
    ASSERT_TRUE(map2);
  }

  SIMPLE_TEST(Inline4) {
    executors::ManualExecutor manual;

    futures::Submit(manual, [&] {
      auto g = futures::Submit(manual, [] {
        return result::Ok(19);
      });
      return result::Ok(std::move(g));
    }) | futures::Flatten() | futures::Detach();

    size_t tasks = manual.Drain();
    ASSERT_EQ(tasks, 2);
  }

  SIMPLE_TEST(Inline5) {
    executors::ManualExecutor manual;

    auto [f1, p1] = futures::Contract<int>();
    auto [f2, p2] = futures::Contract<int>();

    auto first = futures::First(
        std::move(f1) | futures::Via(manual),
        std::move(f2) | futures::Via(manual));

    bool ok = false;

    std::move(first)
        | futures::Map([&ok](int v) {
            ASSERT_EQ(v, 31);
            ok = true;
            return Unit{};
          })
        | futures::Detach();

    std::move(p2).SetError(IoError());
    std::move(p1).SetValue(31);

    size_t tasks = manual.Drain();
    ASSERT_EQ(tasks, 0);

    ASSERT_TRUE(ok);
  }
}

RUN_ALL_TESTS()
