#include <wheels/test/framework.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sched/yield.hpp>

#include "../common/run.hpp"
#include "../common/test.hpp"

#include <chrono>

using namespace exe;
using namespace std::chrono_literals;

//////////////////////////////////////////////////////////////////////

TEST_SUITE(Yield) {
  TEST(Stress, wheels::test::TestOptions().TimeLimit(5s)) {
    RunScheduler(/*threads=*/4, [] {
      for (size_t i = 0; i < 127; ++i) {
        fibers::Go([] {
          while (KeepRunning()) {
            fibers::Yield();
          }
        });
      }
    });
  }
}

RUN_ALL_TESTS()
