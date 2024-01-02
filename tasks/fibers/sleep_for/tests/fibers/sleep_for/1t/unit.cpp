#include <wheels/test/framework.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sched/yield.hpp>
#include <exe/fibers/sched/sleep_for.hpp>

#include <wheels/core/stop_watch.hpp>

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
  SIMPLE_TEST(JustWorks) {
    RunScheduler(/*threads=*/1, []() {
      wheels::StopWatch stop_watch;

      fibers::SleepFor(256ms);

      ASSERT_TRUE(stop_watch.Elapsed() > 100ms);
    });
  }

  SIMPLE_TEST(DoNotYield) {
    asio::io_context scheduler;
    bool done = false;

    fibers::Go(scheduler, [&done] {
      fibers::SleepFor(200ms);
      done = true;
    });

    auto handlers = scheduler.run();

    std::cout << "Handlers: " << handlers << std::endl;

    ASSERT_TRUE(handlers < 10);
    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(For) {
    RunScheduler(/*threads=*/1, []() {
      wheels::StopWatch stop_watch;

      for (size_t i = 0; i < 5; ++i) {
        fibers::SleepFor(1s);
      }

      ASSERT_TRUE(stop_watch.Elapsed() > 4s);
    });
  }

  SIMPLE_TEST(Concurrent) {
    RunScheduler(/*threads=*/1, [] {
      size_t sleepers = 0;
      WaitGroup wg;

      for (size_t i = 0; i < 17; ++i) {
        wg.Add(1);

        fibers::Go([&] {
          fibers::SleepFor(2s);
          ++sleepers;
          wg.Done();
        });
      }

      wg.Wait();

      ASSERT_EQ(sleepers, 17);
    });
  }

  SIMPLE_TEST(NonBlocking) {
    RunScheduler(/*threads=*/1, [] {
      size_t yields = 0;
      bool released = false;

      WaitGroup wg;

      wg.Add(1);

      fibers::Go([&] {
        while (!released) {
          ++yields;
          fibers::Yield();
        }
        wg.Done();
      });

      fibers::SleepFor(256ms);
      released = true;

      wg.Wait();

      ASSERT_TRUE(yields >= 1234);
    });
  }

  SIMPLE_TEST(Delays) {
    RunScheduler(/*threads=*/1, [] {
      for (size_t i = 1; i <= 3; ++i) {
        fibers::Go([i]() {
          const auto delay = i * 1s;

          wheels::StopWatch stop_watch;
          {
            fibers::SleepFor(delay);
          }
          auto elapsed = stop_watch.Elapsed();

          ASSERT_TRUE(elapsed >= delay - 128ms);
          ASSERT_TRUE(elapsed <= delay + 128ms)
        });
      }
    });
  }

  SIMPLE_TEST(Stress) {
    RunScheduler(/*threads=*/1, [] {
      WaitGroup wg;

      for (size_t i = 0; i < 100; ++i) {
        wg.Add(1);

        fibers::Go([&]() {
          for (size_t j = 0; j < 256; ++j) {
            fibers::SleepFor(3ms);
          }
          wg.Done();
        });
      }

      fibers::SleepFor(2s);

      wg.Wait();
    });
  }
}

RUN_ALL_TESTS()
