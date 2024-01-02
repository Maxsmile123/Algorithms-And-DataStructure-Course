#include "../queue_spinlock.hpp"

#include <course/twist/test.hpp>

#include <twist/test/race.hpp>
#include <twist/test/plate.hpp>
#include <twist/test/budget.hpp>

#include <chrono>

using namespace std::chrono_literals;

////////////////////////////////////////////////////////////////////////////////

TEST_SUITE(MutualExclusion) {
  void Test(size_t threads) {
    QueueSpinLock spinlock;
    twist::test::Plate plate;  // Guarded by spinlock

    twist::test::Race race;

    for (size_t i = 0; i < threads; ++i) {
      race.Add([&] {
        for (twist::test::TimeBudget budget; budget; ) {
          QueueSpinLock::Guard guard{spinlock};
          {
            // Critical section
            plate.Access();
          }
        }
      });
    }

    race.Run();

    std::cout << "Critical sections: " << plate.AccessCount() << std::endl;
  }

  TWIST_TEST(Stress2, 5s) {
    Test(2);
  }

  TWIST_TEST(Stress5, 5s) {
    Test(5);
  }
}

////////////////////////////////////////////////////////////////////////////////

TEST_SUITE(MissedWakeup) {
  void Test(size_t threads) {
    QueueSpinLock spinlock;

    twist::test::Race race;

    for (size_t i = 0; i < threads; ++i) {
      race.Add([&] {
        QueueSpinLock::Guard guard{spinlock};
        // Critical section
      });
    };

    race.Run();
  }

  TWIST_TEST_REPEAT(Stress2, 5s) {
    Test(2);
  }

  TWIST_TEST_REPEAT(Stress3, 5s) {
    Test(3);
  }
}

////////////////////////////////////////////////////////////////////////////////

RUN_ALL_TESTS()
