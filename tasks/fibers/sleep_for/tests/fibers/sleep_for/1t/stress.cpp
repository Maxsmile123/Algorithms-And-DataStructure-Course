#include <wheels/test/framework.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sched/sleep_for.hpp>

#include "../../common/run.hpp"
#include "../../common/test.hpp"

using namespace exe;
using namespace std::chrono_literals;

//////////////////////////////////////////////////////////////////////

class WaitGroup {
 public:
  void Add(size_t count) {
    count_ += count;
  }

  void Done() {
    --count_;
  }

  void Wait() {
    while (count_ > 0) {
      fibers::SleepFor(1ms);
    }
  }

 private:
  size_t count_{0};
};

//////////////////////////////////////////////////////////////////////

TEST_SUITE(SleepForOneThread) {
  SIMPLE_TEST(Stress) {
    RunScheduler(/*threads=*/1, [] {
      WaitGroup wg;

      for (size_t i = 0; i < 100; ++i) {
        wg.Add(1);

        fibers::Go([&]() {
          while (KeepRunning()) {
            fibers::SleepFor(3ms);
          }
          wg.Done();
        });
      }

      wg.Wait();
    });
  }
}

RUN_ALL_TESTS()
