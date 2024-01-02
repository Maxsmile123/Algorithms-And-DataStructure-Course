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
  SIMPLE_TEST(JustWorks) {
    RunScheduler(/*threads=*/4, []() {
      for (size_t i = 0; i < 17; ++i) {
        fibers::SleepFor(100ms);
        std::cout << i << std::endl;
      }
    });
  }
}

RUN_ALL_TESTS()
