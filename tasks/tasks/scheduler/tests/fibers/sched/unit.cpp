#include <exe/executors/thread_pool.hpp>
#include <exe/executors/manual.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sched/yield.hpp>
#include <exe/threads/wait_group.hpp>

#include <wheels/core/stop_watch.hpp>

#include <wheels/test/framework.hpp>

#include <thread>

using namespace exe;

TEST_SUITE(Fibers) {
  SIMPLE_TEST(Go) {
    executors::ManualExecutor scheduler;

    bool done = false;

    fibers::Go(scheduler, [&] {
      done = true;
    });

    ASSERT_FALSE(done);

    size_t tasks = scheduler.Drain();

    ASSERT_EQ(tasks, 1);
    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(Child) {
    executors::ManualExecutor scheduler;

    size_t done = 0;

    fibers::Go(scheduler, [&] {
      fibers::Go([&] {
        ++done;
      });

      ++done;
    });

    size_t tasks = scheduler.Drain();

    ASSERT_EQ(tasks, 2);
    ASSERT_EQ(done, 2);
  }

  SIMPLE_TEST(RunInParallel) {
    executors::ThreadPool pool{3};
    pool.Start();

    threads::WaitGroup wg;

    auto sleeper = [&]() {
      std::this_thread::sleep_for(3s);
      wg.Done();
    };

    wheels::StopWatch stop_watch;

    wg.Add(3);

    fibers::Go(pool, sleeper);
    fibers::Go(pool, sleeper);
    fibers::Go(pool, sleeper);

    wg.Wait();

    ASSERT_TRUE(stop_watch.Elapsed() < 3s + 500ms);

    pool.Stop();
  }

  SIMPLE_TEST(Yield1) {
    executors::ManualExecutor scheduler;

    bool done = false;

    fibers::Go(scheduler, [&] {
      fibers::Yield();
      done = true;
    });

    ASSERT_FALSE(done);

    scheduler.RunNext();
    ASSERT_FALSE(done);

    scheduler.RunNext();
    ASSERT_TRUE(done);

    ASSERT_FALSE(scheduler.RunNext());
  }

  SIMPLE_TEST(Yield2) {
    executors::ManualExecutor scheduler;

    enum State : int {
      Ping = 0,
      Pong = 1
    };

    int state = Ping;

    fibers::Go(scheduler, [&] {
      for (size_t i = 0; i < 2; ++i) {
        ASSERT_EQ(state, Ping);
        state = Pong;

        fibers::Yield();
      }
    });

    fibers::Go(scheduler, [&] {
      for (size_t j = 0; j < 2; ++j) {
        ASSERT_EQ(state, Pong);
        state = Ping;

        fibers::Yield();
      }
    });

    size_t tasks = scheduler.Drain();

    ASSERT_EQ(tasks, 6);
  }

  SIMPLE_TEST(Yield3) {
    executors::ThreadPool pool{4};

    static const size_t kYields = 1024;

    threads::WaitGroup wg;
    wg.Add(2);

    auto runner = [&wg] {
      for (size_t i = 0; i < kYields; ++i) {
        fibers::Yield();
      }
      wg.Done();
    };

    fibers::Go(pool, runner);
    fibers::Go(pool, runner);

    pool.Start();

    wg.Wait();

    pool.Stop();
  }

  SIMPLE_TEST(TwoSchedulers) {
    executors::ManualExecutor scheduler_1;
    executors::ManualExecutor scheduler_2;

    fibers::Go(scheduler_1, []{
      fibers::Yield();
    });

    fibers::Go(scheduler_2, []{
      fibers::Yield();
      fibers::Yield();
    });

    ASSERT_EQ(scheduler_1.Drain(), 2);
    ASSERT_EQ(scheduler_2.Drain(), 3);
  }
}

RUN_ALL_TESTS()
