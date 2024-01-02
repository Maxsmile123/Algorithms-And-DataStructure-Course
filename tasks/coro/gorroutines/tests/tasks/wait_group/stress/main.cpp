#include <wheels/test/framework.hpp>
#include <twist/test/budget.hpp>

#include <course/twist/test.hpp>

#include <exe/executors/thread_pool.hpp>

#include <exe/gorr/core/task.hpp>
#include <exe/gorr/run/fire.hpp>
#include <exe/gorr/run/teleport.hpp>
#include <exe/gorr/sync/wait_group.hpp>

#include <atomic>
#include <chrono>

using namespace exe;
using namespace std::chrono_literals;

//////////////////////////////////////////////////////////////////////

using Scheduler = executors::ThreadPool;

//////////////////////////////////////////////////////////////////////

void StressTest1(size_t workers, size_t waiters) {
  Scheduler scheduler{/*threads=*/4};

  while (twist::test::KeepRunning()) {
    gorr::WaitGroup wg;

    // Number of completed waiters
    std::atomic<size_t> waiters_done{0};
    std::atomic<size_t> workers_done{0};

    wg.Add(workers);

    // Waiters

    auto waiter = [&]() -> gorr::Task<> {
      co_await gorr::TeleportTo(scheduler);

      co_await wg.Wait();
      waiters_done.fetch_add(1);
    };

    for (size_t i = 0; i < waiters; ++i) {
      gorr::FireAndForget(waiter());
    }

    // Workers

    auto worker = [&]() -> gorr::Task<> {
      co_await gorr::TeleportTo(scheduler);

      workers_done.fetch_add(1);
      wg.Done();
    };

    for (size_t j = 0; j < workers; ++j) {
      gorr::FireAndForget(worker());
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
  explicit Goer(Scheduler& scheduler, gorr::WaitGroup& wg)
      : scheduler_(scheduler), wg_(wg) {
  }

  void Start(size_t steps) {
    steps_left_ = steps;
    Step();
  }

  size_t Steps() const {
    return steps_made_;
  }

 private:
  gorr::Task<> NextStep() {
    co_await gorr::TeleportTo(scheduler_);

    Step();
    wg_.Done();

    co_return;
  }

  void Step() {
    if (steps_left_ == 0) {
      return;
    }

    ++steps_made_;
    --steps_left_;

    wg_.Add(1);
    gorr::FireAndForget(NextStep());
  }

 private:
  Scheduler& scheduler_;
  gorr::WaitGroup& wg_;
  size_t steps_left_;
  size_t steps_made_ = 0;
};

void StressTest2() {
  Scheduler scheduler{/*threads=*/4};

  size_t iter = 0;

  while (twist::test::KeepRunning()) {
    ++iter;

    bool done = false;

    auto tester = [&scheduler, &done, iter]() -> gorr::Task<> {
      const size_t steps = 1 + iter % 3;

      // Размещаем wg на куче, но только для того, чтобы
      // AddressSanitizer мог обнаружить ошибку
      // Можно считать, что wg находится на стеке
      auto wg = std::make_unique<gorr::WaitGroup>();
      //fibers::WaitGroup wg;

      Goer goer{scheduler, *wg};
      goer.Start(steps);

      co_await wg->Wait();

      ASSERT_EQ(goer.Steps(), steps);
      ASSERT_TRUE(steps > 0);

      done = true;
    };

    gorr::FireAndForget(tester());

    scheduler.WaitIdle();

    ASSERT_TRUE(done);
  }

  scheduler.Stop();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(GorrWaitGroup) {
  TWIST_TEST(Stress1_1_1, 5s) {
    StressTest1(/*workers=*/1, /*waiters=*/1);
  }

  TWIST_TEST(Stress1_2_2, 5s) {
    StressTest1(/*workers=*/2, /*waiters=*/2);
  }

  TWIST_TEST(Stress1_3_3, 5s) {
    StressTest1(/*workers=*/3, /*waiters=*/3);
  }

  TWIST_TEST(Stress2, 5s) {
    StressTest2();
  }
}

RUN_ALL_TESTS()
