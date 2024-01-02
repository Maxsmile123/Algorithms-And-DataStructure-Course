#include "../cyclic_barrier.hpp"

#include <course/twist/test.hpp>

#include <twist/test/race.hpp>
#include <twist/test/budget.hpp>

#include <twist/ed/stdlike/thread.hpp>

////////////////////////////////////////////////////////////////////////////////

void TestLeader(const size_t threads) {
  CyclicBarrier barrier{threads};
  size_t leader = 0;
  bool stop = false;

  twist::test::Race race;

  for (size_t i = 0; i < threads; ++i) {
    race.Add([&, i]() {
      barrier.ArriveAndWait();

      for (size_t k = 0; ; ++k) {
        // Rotating leader writes to shared variable
        if (k % threads == i) {
          leader = k;
          if (!twist::test::KeepRunning()) {
            stop = true;
          }
        } else {
          twist::ed::stdlike::this_thread::yield();
        }

        barrier.ArriveAndWait();

        // All threads read from shared variable
        ASSERT_EQ(leader, k);

        if (stop) {
          break;
        }

        barrier.ArriveAndWait();
      }
    });
  };

  race.Run();
}

TWIST_TEST_TEMPLATE(RotatingLeader, TestLeader)
    ->TimeLimit(5s)
    ->Run(2)
    ->Run(5)
    ->Run(10);

////////////////////////////////////////////////////////////////////////////////

void TestWaves(size_t threads, size_t waves) {
  CyclicBarrier barrier{threads};

  twist::test::Race race;

  for (size_t i = 0; i < threads; ++i) {
    race.Add([&] {
      for (size_t j = 0; j < waves; ++j) {
        barrier.ArriveAndWait();
      }
    });
  }

  race.Run();
}

TEST_SUITE(Waves) {
  TWIST_TEST_REPEAT(Waves_2_2, 5s) {
    TestWaves(2, 2);
  }

  TWIST_TEST_REPEAT(Waves_3_3, 5s) {
    TestWaves(3, 3);
  }

  TWIST_TEST_REPEAT(Waves_4_4, 5s) {
    TestWaves(4, 4);
  }
}

////////////////////////////////////////////////////////////////////////////////

RUN_ALL_TESTS()
