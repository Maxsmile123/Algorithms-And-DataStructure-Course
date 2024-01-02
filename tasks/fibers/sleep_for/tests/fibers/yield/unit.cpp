#include <wheels/test/framework.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sched/yield.hpp>

#include "../common/run.hpp"

using namespace exe;

//////////////////////////////////////////////////////////////////////

TEST_SUITE(Yield) {
  SIMPLE_TEST(JustWorks) {
    RunScheduler(/*threads=*/1, [] {
      for (size_t i = 0; i < 3; ++i) {
        fibers::Yield();
      }
    });
  }

  SIMPLE_TEST(PingPong) {
    RunScheduler(/*threads=*/1, [] {
      size_t step = 0;

      fibers::Go([&] {
        for (size_t j = 1; j < 7; ++j) {
          step = j;
          fibers::Yield();
        }
      });

      for (size_t i = 1; i < 7; ++i) {
        fibers::Yield();
        ASSERT_EQ(step, i);
      }
    });
  }
}

RUN_ALL_TESTS()
