#include "mutexed.hpp"

#include <course/twist/test.hpp>

#include <twist/test/race.hpp>
#include <twist/test/plate.hpp>
#include <twist/test/budget.hpp>

TEST_SUITE(Mutexed) {
  TWIST_TEST(Stress, 3s) {
    static const size_t kThreads = 3;

    // Set of hungry threads
    twist::test::Race race;
    // Plate shared between threads
    Mutexed<twist::test::Plate> plate;

    for (size_t i = 0; i < kThreads; ++i) {
      race.Add([&]() {
        while (twist::test::KeepRunning()) {
          Acquire(plate)->Access();
        }
      });
    }

    race.Run();
  }
}

RUN_ALL_TESTS()
