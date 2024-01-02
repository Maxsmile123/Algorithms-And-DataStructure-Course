#include "../spinlock.hpp"

#include <course/twist/test.hpp>

#include <twist/test/race.hpp>
#include <twist/test/plate.hpp>
#include <twist/test/budget.hpp>

#include <twist/ed/wait/spin.hpp>

#include <chrono>

////////////////////////////////////////////////////////////////////////////////

using namespace std::chrono_literals;

////////////////////////////////////////////////////////////////////////////////

TEST_SUITE(SpinLock) {
  void Test(size_t lockers, size_t try_lockers) {
    twist::test::Plate plate;  // Guarded by spinlock
    TASSpinLock spinlock;

    twist::test::Race race;

    std::cout << "Lockers: " << lockers
      << ", try_lockers: " << try_lockers << std::endl;

    for (size_t i = 0; i < lockers; ++i) {
      race.Add([&]() {
        while (twist::test::KeepRunning()) {
          spinlock.Lock();
          plate.Access();
          spinlock.Unlock();
        }
      });
    }

    for (size_t j = 0; j < try_lockers; ++j) {
      race.Add([&]() {
        while (twist::test::KeepRunning()) {
          twist::ed::SpinWait spin_wait;
          while (!spinlock.TryLock()) {
            spin_wait();
          }
          plate.Access();
          spinlock.Unlock();
        }
      });
    }

    race.Run();

    std::cout << "Critical sections: " << plate.AccessCount() << std::endl;
  }

  TWIST_TEST(Stress1, 5s) {
    Test(3, 0);
  }

  TWIST_TEST(Stress2, 5s) {
    Test(0, 3);
  }

  TWIST_TEST(Stress3, 5s) {
    Test(3, 3);
  }

  TWIST_TEST(Stress4, 10s) {
    Test(5, 5);
  }

  TWIST_TEST(Stress5, 10s) {
    Test(10, 10);
  }
}

RUN_ALL_TESTS()