#include <wheels/test/framework.hpp>

#include <stdlike/promise.hpp>

#include <string>
#include <thread>
#include <variant>

using stdlike::Promise;
using stdlike::Future;

using namespace std::chrono_literals;

TEST_SUITE(Futures) {
  SIMPLE_TEST(GetValue) {
    Promise<int> p;
    Future<int> f = p.MakeFuture();

    p.SetValue(42);
    ASSERT_EQ(f.Get(), 42);
  }

  struct TestException {
  };

  SIMPLE_TEST(ThrowException) {
    Promise<int> p;
    auto f = p.MakeFuture();

    try {
      throw TestException();
    } catch (...) {
      p.SetException(std::current_exception());
    }

    ASSERT_THROW(f.Get(), TestException)
  }

  SIMPLE_TEST(WaitValue) {
    Promise<std::string> p;
    auto f = p.MakeFuture();

    std::thread producer([p = std::move(p)]() mutable {
      std::this_thread::sleep_for(1s);
      p.SetValue("Hi");
    });

    ASSERT_EQ(f.Get(), "Hi");

    producer.join();
  }

  SIMPLE_TEST(WaitException) {
    Promise<std::string> p;
    auto f = p.MakeFuture();

    std::thread producer([p = std::move(p)]() mutable {
      std::this_thread::sleep_for(1s);
      try {
        throw TestException();
      } catch (...) {
        p.SetException(std::current_exception());
      }
    });

    ASSERT_THROW(f.Get(), TestException);

    producer.join();
  }

  template <typename T>
  void Drop(T) {
  }

  SIMPLE_TEST(DropFuture) {
    Promise<std::string> p;
    auto f = p.MakeFuture();

    Drop(std::move(f));

    std::thread producer([p = std::move(p)]() mutable {
      p.SetValue("Hi");
    });

    producer.join();
  }

  SIMPLE_TEST(DropPromise) {
    Promise<std::string> p;
    auto f = p.MakeFuture();

    p.SetValue("Test");
    Drop(std::move(p));

    ASSERT_EQ(f.Get(), "Test");
  }

  SIMPLE_TEST(Futures) {
    Promise<int> p0;
    Promise<int> p1;
    Promise<int> p2;

    auto f0 = p0.MakeFuture();
    auto f1 = p1.MakeFuture();
    auto f2 = p2.MakeFuture();

    std::thread producer0([&]() {
      std::this_thread::sleep_for(3s);
      p0.SetValue(0);
    });

    std::thread producer1([&]() {
      std::this_thread::sleep_for(1s);
      p1.SetValue(1);
    });

    std::thread producer2([&]() {
      std::this_thread::sleep_for(2s);
      p2.SetValue(2);
    });

    ASSERT_EQ(f0.Get(), 0);
    ASSERT_EQ(f1.Get(), 1);
    ASSERT_EQ(f2.Get(), 2);

    producer0.join();
    producer1.join();
    producer2.join();
  }
}

RUN_ALL_TESTS()
