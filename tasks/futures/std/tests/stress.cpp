#include <stdlike/promise.hpp>

#include <course/twist/test.hpp>
#include <twist/test/race.hpp>
#include <twist/test/random.hpp>

#include <string>

using namespace std::chrono_literals;

using stdlike::Promise;
using stdlike::Future;

TEST_SUITE(Future) {
  TWIST_TEST_REPEAT(Rendezvous, 5s) {
    Promise<int> p;
    auto f = p.MakeFuture();

    twist::test::Race race;

    race.Add([p = std::move(p)]() mutable {
      p.SetValue(17);
    });

    race.Add([f = std::move(f)]() mutable {
      ASSERT_EQ(f.Get(), 17);
    });

    race.Run();
  }

  TWIST_TEST_REPEAT(ConcurrentRendezvous, 5s) {
    // Contracts

    Promise<std::string> p0;
    auto f0 = p0.MakeFuture();

    Promise<std::string> p1;
    auto f1 = p1.MakeFuture();

    // Race

    twist::test::Race race;

    // Producers

    race.Add([p = std::move(p0)]() mutable {
      p.SetValue("Hello");
    });

    race.Add([p = std::move(p1)]() mutable {
      p.SetValue("World");
    });

    // Consumers

    race.Add([f = std::move(f0)]() mutable {
      ASSERT_EQ(f.Get(), "Hello");
    });

    race.Add([f = std::move(f1)]() mutable {
      ASSERT_EQ(f.Get(), "World");
    });

    race.Run();
  }

  template <typename T>
  void Drop(T) {
  }

  TWIST_TEST_REPEAT(SharedState, 5s) {
    // Make contract
    Promise<std::string> p;
    Future<std::string> f = p.MakeFuture();

    // Run concurrent producer & consumer

    twist::test::Race race;

    race.Add([f = std::move(f)]() mutable {
      if (twist::test::Random2()) {
        ASSERT_EQ(f.Get(), "Test");
      }
      Drop(std::move(f));
    });

    race.Add([p = std::move(p)]() mutable {
      p.SetValue("Test");
      Drop(std::move(p));
    });

    race.Run();
  }
}

RUN_ALL_TESTS()
