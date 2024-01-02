#include <task/task.hpp>
#include <task/await.hpp>

#include "runtime.hpp"

#include <wheels/test/framework.hpp>

using task::Task;
using task::Await;

TEST_SUITE(Task) {
  Task<int> Bar() {
    std::cout << "Bar" << std::endl;
    co_return 7;
  }

  SIMPLE_TEST(JustWorks) {
    auto task = Bar();
    int value = Await(std::move(task));
    ASSERT_EQ(value, 7);
  }

  Task<int> Lazy(bool& started) {
    started = true;
    co_return 5;
  }

  SIMPLE_TEST(Lazy) {
    bool started = false;
    auto task = Lazy(started);
    ASSERT_FALSE(started);
    int value = Await(std::move(task));
    ASSERT_TRUE(started);
    ASSERT_EQ(value, 5);
  }

  Task<int> Foo() {
    std::cout << "Foo" << std::endl;
    int value = co_await Bar();
    co_return value;
  }

  SIMPLE_TEST(Compose1) {
    auto task = Foo();
    int value = Await(std::move(task));

    ASSERT_EQ(value, 7);
  }

  Task<std::string> Baz() {
    int value = co_await Bar();
    co_return std::to_string(value);
  }

  SIMPLE_TEST(Compose2) {
    auto task = Baz();
    std::string str = Await(std::move(task));

    ASSERT_EQ(str, "7");
  }

  struct TestError : std::runtime_error {
    TestError() : std::runtime_error("Test") {}
  };

  Task<int> Throw() {
    throw TestError{};
    co_return 7;
  }

  Task<int> Catch1() {
    ASSERT_THROW(co_await Throw(), TestError);
    co_return 7;
  }

  SIMPLE_TEST(Exceptions1) {
    task::Await(Catch1());
  }

  Task<int> Rethrow() {
    int value = co_await Throw();
    FAIL_TEST("Still running!");
    co_return value;
  }

  Task<int> Catch2() {
    ASSERT_THROW(co_await Rethrow(), TestError);
    co_return 42;
  }

  SIMPLE_TEST(Exceptions2) {
    Await(Catch2());
  }

  SIMPLE_TEST(Exceptions3) {
    ASSERT_THROW(Await(Throw()), TestError);
  }

  Task<int> Recurse(size_t calls) {
    if (calls > 0) {
      co_await Recurse(calls - 1);
    }
    co_return 123;
  }

  SIMPLE_TEST(Recurse1) {
    Task task = Recurse(17);
    int value = Await(std::move(task));
    ASSERT_EQ(value, 123);
  }

  Task<size_t> Step(size_t i) {
    co_return i;
  }

  Task<size_t> Sum(size_t n) {
    size_t total = 0;
    for (size_t i = 0; i < n; ++i) {
      total += co_await Step(i);
    }
    co_return total;
  }

  TEST(Loop, wheels::test::TestOptions().AdaptTLToSanitizer()) {
    static const size_t kN = 1'000'000;
    static const size_t kTotalExpected = kN * (kN - 1) / 2;

    size_t total = Await(Sum(kN));

    ASSERT_EQ(total, kTotalExpected);
  }

  /*
  struct MoveOnly {
    MoveOnly() = default;

    // Non-copyable
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;

    // Movable
    MoveOnly(MoveOnly&&) = default;
    MoveOnly& operator=(MoveOnly&&) = default;
  };

  Task<MoveOnly> Move() {
    co_return MoveOnly{};
  }

  SIMPLE_TEST(MoveOnly) {
    MoveOnly value = Await(Move());
  }
  */

  Task<int> Fib(tp::StaticThreadPool& pool, int k) {
    co_await ScheduleTo(pool);

    ASSERT_EQ(tp::StaticThreadPool::Current(), &pool);

    if (k > 1) {
      Task t1 = Fib(pool, k - 1);
      Task t2 = Fib(pool, k - 2);
      int f1 = co_await t1;
      int f2 = co_await t2;
      co_return f1 + f2;
    } else {
      co_return 1;
    }
  }

  SIMPLE_TEST(Fib) {
    tp::StaticThreadPool pool{4};
    auto task = Fib(pool, 19);
    int f = Await(std::move(task));

    std::cout << "Fib(19) = " << f << std::endl;

    ASSERT_EQ(f, 6765);

    pool.Join();
  }
}
