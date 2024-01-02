#include <wheels/test/framework.hpp>

#include <task/join.hpp>
#include <task/join_handle.hpp>

#include "runtime.hpp"

#include <atomic>

using task::JoinHandle;
using task::Join;

TEST_SUITE(JoinHandle) {
  // Shared among tests =(
  bool ok = false;

  JoinHandle Bar() {
    std::cout << "Bar" << std::endl;
    ok = true;
    co_return;
  }

  SIMPLE_TEST(JustWorks) {
    ok = false;

    JoinHandle h = Bar();
    ASSERT_FALSE(ok);  // Lazy
    Join(std::move(h));

    ASSERT_TRUE(ok);
  }

  JoinHandle Foo() {
    std::cout << "Foo" << std::endl;
    co_await Bar();
    co_return;
  }

  SIMPLE_TEST(Compose) {
    ok = false;

    JoinHandle h = Foo();
    Join(std::move(h));

    ASSERT_TRUE(ok);
  }

  struct TestError : std::runtime_error {
    TestError() : std::runtime_error("Test") {}
  };

  JoinHandle Throw() {
    throw TestError{};
    co_return;
  }

  JoinHandle Catch1() {
    ASSERT_THROW(co_await Throw(), TestError);
    ok = true;
    co_return;
  }

  SIMPLE_TEST(Exceptions1) {
    ok = false;
    Join(Catch1());
    ASSERT_TRUE(ok);
  }

  JoinHandle Rethrow() {
    co_await Throw();
    co_return;
  }

  JoinHandle Catch2() {
    ASSERT_THROW(co_await Rethrow(), TestError);
    ok = true;
  }

  SIMPLE_TEST(Exceptions2) {
    ok = false;
    Join(Catch2());
    ASSERT_TRUE(ok);
  }

  SIMPLE_TEST(Exceptions3) {
    ASSERT_THROW(Join(Throw()), TestError);
  }

  JoinHandle Recurse(size_t calls) {
    if (calls > 0) {
      co_await Recurse(calls - 1);
    } else {
      ok = true;
    }
    co_return;
  }

  SIMPLE_TEST(Recurse1) {
    ok = false;
    Join(Recurse(17));
    ASSERT_TRUE(ok);
  }

  JoinHandle Step() {
    co_return;
  }

  JoinHandle Loop(size_t n) {
    int steps = 0;
    for (size_t i = 0; i < n; ++i) {
      co_await Step();
      ++steps;
    }
    ok = true;
    co_return;
  }

  TEST(Loop, wheels::test::TestOptions().AdaptTLToSanitizer()) {
    ok = false;
    Join(Loop(1'000'000));
    ASSERT_TRUE(ok);
  }

  std::atomic<int> f_value = 0;

  JoinHandle Fib(tp::StaticThreadPool& pool, int k) {
    co_await ScheduleTo(pool);

    ASSERT_EQ(tp::StaticThreadPool::Current(), &pool);

    if (k > 1) {
      auto h1 = Fib(pool, k - 1);
      auto h2 = Fib(pool, k - 2);
      co_await h1;
      co_await h2;
    } else {
      ++f_value;
    }
    co_return;
  }

  SIMPLE_TEST(Fib) {
    f_value = 0;

    tp::StaticThreadPool pool{4};
    Join(Fib(pool, 19));

    std::cout << "Fib(19) = " << f_value << std::endl;

    ASSERT_EQ(f_value, 6765);

    pool.Join();
  }
}
