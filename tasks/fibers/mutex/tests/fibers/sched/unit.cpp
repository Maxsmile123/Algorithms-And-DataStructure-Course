#include <exe/executors/thread_pool.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sched/yield.hpp>

#include <wheels/core/stop_watch.hpp>

#include <wheels/test/framework.hpp>

#include <thread>

using namespace exe;

void AssertOn(executors::ThreadPool& pool) {
  ASSERT_EQ(executors::ThreadPool::Current(), &pool);
}

TEST_SUITE(Fibers) {
  SIMPLE_TEST(JustWorks) {
    executors::ThreadPool pool{3};
    pool.Start();

    bool done = false;

    fibers::Go(pool, [&]() {
      AssertOn(pool);
      done = true;
    });

    pool.WaitIdle();

    ASSERT_TRUE(done);

    pool.Stop();
  }

  SIMPLE_TEST(Child) {
    executors::ThreadPool pool{3};
    pool.Start();

    std::atomic<size_t> done{0};

    auto init = [&]() {
      AssertOn(pool);

      fibers::Go([&]() {
        AssertOn(pool);
        ++done;
      });

      ++done;
    };

    fibers::Go(pool, init);

    pool.WaitIdle();

    ASSERT_EQ(done.load(), 2);

    pool.Stop();
  }

  SIMPLE_TEST(RunInParallel) {
    executors::ThreadPool pool{3};
    pool.Start();

    std::atomic<size_t> completed{0};

    auto sleeper = [&]() {
      std::this_thread::sleep_for(3s);
      completed.fetch_add(1);
    };

    wheels::StopWatch stop_watch;

    fibers::Go(pool, sleeper);
    fibers::Go(pool, sleeper);
    fibers::Go(pool, sleeper);

    pool.WaitIdle();

    ASSERT_EQ(completed.load(), 3);
    ASSERT_TRUE(stop_watch.Elapsed() < 3s + 500ms);

    pool.Stop();
  }

  SIMPLE_TEST(Yield1) {
    executors::ThreadPool pool{1};
    pool.Start();

    bool done = false;

    fibers::Go(pool, [&] {
      fibers::Yield();

      AssertOn(pool);
      done = true;
    });

    pool.WaitIdle();
    ASSERT_TRUE(done);

    pool.Stop();
  }

  SIMPLE_TEST(Yield2) {
    executors::ThreadPool pool{1};

    enum State : int {
      Ping = 0,
      Pong = 1
    };

    int state = Ping;

    fibers::Go(pool, [&] {
      for (size_t i = 0; i < 2; ++i) {
        ASSERT_EQ(state, Ping);
        state = Pong;

        fibers::Yield();
      }
    });

    fibers::Go(pool, [&] {
      for (size_t j = 0; j < 2; ++j) {
        ASSERT_EQ(state, Pong);
        state = Ping;

        fibers::Yield();
      }
    });

    pool.Start();

    pool.WaitIdle();
    pool.Stop();
  }

  SIMPLE_TEST(Yield3) {
    executors::ThreadPool pool{4};

    static const size_t kYields = 1024;

    auto runner = [] {
      for (size_t i = 0; i < kYields; ++i) {
        fibers::Yield();
      }
    };

    fibers::Go(pool, runner);
    fibers::Go(pool, runner);

    pool.Start();

    pool.WaitIdle();
    pool.Stop();
  }

  SIMPLE_TEST(TwoPools1) {
    executors::ThreadPool pool_1{4};
    executors::ThreadPool pool_2{4};

    pool_1.Start();
    pool_2.Start();

    auto make_tester = [](executors::ThreadPool& pool) {
      return [&pool]() {
        AssertOn(pool);
      };
    };

    fibers::Go(pool_1, make_tester(pool_1));
    fibers::Go(pool_2, make_tester(pool_2));

    pool_1.WaitIdle();
    pool_2.WaitIdle();

    pool_1.Stop();
    pool_2.Stop();
  }

  SIMPLE_TEST(TwoPools2) {
    executors::ThreadPool pool_1{4};
    pool_1.Start();

    executors::ThreadPool pool_2{4};
    pool_2.Start();

    auto make_tester = [](executors::ThreadPool& pool) {
      return [&pool]() {
        static const size_t kIterations = 128;

        for (size_t i = 0; i < kIterations; ++i) {
          AssertOn(pool);

          fibers::Yield();

          fibers::Go(pool, [&pool]() {
            AssertOn(pool);
          });
        }
      };
    };

    fibers::Go(pool_1, make_tester(pool_1));
    fibers::Go(pool_2, make_tester(pool_2));

    pool_1.WaitIdle();
    pool_2.WaitIdle();

    pool_1.Stop();
    pool_2.Stop();
  }
}

RUN_ALL_TESTS()
