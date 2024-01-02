#include <exe/executors/thread_pool.hpp>

#include <exe/fibers/core/api.hpp>
#include <exe/fibers/sync/wait_group.hpp>

#include <wheels/test/framework.hpp>
#include <twist/test/budget.hpp>

#include <course/twist/test.hpp>

#include <atomic>
#include <chrono>

using namespace exe;

using namespace std::chrono_literals;

//////////////////////////////////////////////////////////////////////

void StressTest1(size_t workers, size_t waiters) {
  executors::ThreadPool scheduler{/*threads=*/4};

  while (twist::test::KeepRunning()) {
    fibers::WaitGroup wg;

    std::atomic<size_t> waiters_done{0};
    std::atomic<size_t> workers_done{0};

    wg.Add(workers);

    // Waiters

    for (size_t i = 0; i < waiters; ++i) {
      fibers::Go(scheduler, [&]() {
        wg.Wait();
        waiters_done.fetch_add(1);
      });
    }

    // Workers

    for (size_t j = 0; j < workers; ++j) {
      fibers::Go(scheduler, [&]() {
        workers_done.fetch_add(1);
        wg.Done();
      });
    }

    scheduler.WaitIdle();

    ASSERT_EQ(waiters_done.load(), waiters);
    ASSERT_EQ(workers_done.load(), workers);
  }

  scheduler.Stop();
}

//////////////////////////////////////////////////////////////////////

class Goer {
 public:
  explicit Goer(fibers::WaitGroup& wg)
  : wg_(wg) {
  }

  void Start(size_t steps) {
    steps_left_ = steps;
    Step();
  }

  size_t Steps() const {
    return steps_made_;
  }

 private:
  void Step() {
    if (steps_left_ == 0) {
      return;
    }

    ++steps_made_;
    --steps_left_;

    wg_.Add(1);
    fibers::Go([this]() {
      Step();
      wg_.Done();
    });
  }

 private:
  fibers::WaitGroup& wg_;
  size_t steps_left_;
  size_t steps_made_ = 0;
};

void StressTest2() {
  executors::ThreadPool scheduler{/*threads=*/4};

  size_t iter = 0;

  while (twist::test::KeepRunning()) {
    ++iter;

    bool done = false;

    fibers::Go(scheduler, [&done, iter]() {
      const size_t steps = 1 + iter % 3;

      // Размещаем wg на куче, но только для того, чтобы
      // AddressSanitizer мог обнаружить ошибку
      // Можно считать, что wg находится на стеке
      auto wg = std::make_unique<fibers::WaitGroup>();
      //fibers::WaitGroup wg;

      Goer goer{*wg};
      goer.Start(steps);

      wg->Wait();

      ASSERT_EQ(goer.Steps(), steps);

      done = true;
    });

    scheduler.WaitIdle();

    ASSERT_TRUE(done);
  }

  scheduler.Stop();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(WaitGroup) {
  TWIST_TEST(Stress_1_1, 5s) {
    StressTest1(/*workers=*/1, /*waiters=*/1);
  }

  TWIST_TEST(Stress_1_2, 5s) {
    StressTest1(/*workers=*/2, /*waiters=*/2);
  }

  TWIST_TEST(Stress_1_3, 5s) {
    StressTest1(/*workers=*/3, /*waiters=*/1);
  }

  TWIST_TEST(Stress_2, 5s) {
    StressTest2();
  }
}

RUN_ALL_TESTS()
