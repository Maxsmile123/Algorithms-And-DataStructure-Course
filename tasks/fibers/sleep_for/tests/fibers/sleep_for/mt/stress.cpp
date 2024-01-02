#include <wheels/test/framework.hpp>
#include <wheels/test/current.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sched/yield.hpp>
#include <exe/fibers/sched/sleep_for.hpp>

#include "../../common/run.hpp"
#include "../../common/test.hpp"

using namespace exe;
using namespace std::chrono_literals;

//////////////////////////////////////////////////////////////////////

TEST_SUITE(SleepForMultiThread) {

  void StressTest1(size_t fibers) {
    RunScheduler(/*threads=*/4, [fibers]() {
      for (size_t i = 0; i < fibers; ++i) {
        fibers::Go([i] {
          for (size_t j = 0; KeepRunning(); ++j) {
            fibers::SleepFor(((i + j) % 5) * 1ms);

            if (j % 11 == 0) {
              fibers::Yield();
            }
          }
        });
      }
    });
  }

  TEST(Stress1_1, wheels::test::TestOptions().TimeLimit(5s)) {
    StressTest1(/*fibers=*/2);
  }

  TEST(Stress1_2, wheels::test::TestOptions().TimeLimit(5s)) {
    StressTest1(/*fibers=*/5);
  }

  TEST(Stress1_3, wheels::test::TestOptions().TimeLimit(5s)) {
    StressTest1(/*fibers=*/10);
  }

  void StressTest2(size_t fibers) {
    while (KeepRunning()) {
      RunScheduler(/*threads=*/4, [fibers]() {
        for (size_t i = 0; i < fibers; ++i) {
          fibers::Go([i] {
            fibers::SleepFor((i % 2) * 1ms);
          });
        }
      });
    }
  }

  TEST(Stress_2_1, wheels::test::TestOptions().TimeLimit(5s)) {
    StressTest2(/*fibers=*/1);
  }

  TEST(Stress_2_2, wheels::test::TestOptions().TimeLimit(5s)) {
    StressTest2(/*fibers=*/2);
  }
}

RUN_ALL_TESTS()
