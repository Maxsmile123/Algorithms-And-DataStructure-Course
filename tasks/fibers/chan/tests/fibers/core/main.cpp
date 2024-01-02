#include <exe/executors/thread_pool.hpp>

#include <exe/fibers/core/api.hpp>

#include <wheels/test/framework.hpp>

#include <thread>

using namespace exe;

void ExpectPool(executors::ThreadPool& pool) {
  ASSERT_EQ(executors::ThreadPool::Current(), &pool);
}

TEST_SUITE(Fibers) {
  SIMPLE_TEST(JustWorks) {
    executors::ThreadPool pool{3};

    bool done = false;

    fibers::Go(pool, [&]() {
      ExpectPool(pool);
      done = true;
    });

    pool.WaitIdle();

    ASSERT_TRUE(done);

    pool.Stop();
  }

  SIMPLE_TEST(Child) {
    executors::ThreadPool pool{3};
    std::atomic<size_t> done{0};

    auto init = [&]() {
      ExpectPool(pool);

      fibers::Go([&]() {
        ExpectPool(pool);
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

  SIMPLE_TEST(Yield) {
    std::atomic<int> value{0};

    auto check_value = [&]() {
      const int kLimit = 10;

      ASSERT_TRUE(value.load() < kLimit);
      ASSERT_TRUE(value.load() > -kLimit);
    };

    static const size_t kIterations = 12345;

    auto bull = [&]() {
      for (size_t i = 0; i < kIterations; ++i) {
        value.fetch_add(1);
        fibers::self::Yield();
        check_value();
      }
    };

    auto bear = [&]() {
      for (size_t i = 0; i < kIterations; ++i) {
        value.fetch_sub(1);
        fibers::self::Yield();
        check_value();
      }
    };

    // NB: 1 worker thread!
    executors::ThreadPool pool{1};

    fibers::Go(pool, [&]() {
      fibers::Go(bull);
      fibers::Go(bear);
    });

    pool.WaitIdle();
    pool.Stop();
  }

  SIMPLE_TEST(Yield2) {
    executors::ThreadPool pool{4};

    static const size_t kYields = 65536;

    auto tester = []() {
      for (size_t i = 0; i < kYields; ++i) {
        fibers::self::Yield();
      }
    };

    fibers::Go(pool, tester);
    fibers::Go(pool, tester);

    pool.WaitIdle();
    pool.Stop();
  }

  class ForkTester {
   public:
    ForkTester(size_t threads) : pool_(threads) {
    }

    size_t Explode(size_t d) {
      fibers::Go(pool_, MakeForker(d));

      pool_.WaitIdle();
      pool_.Stop();

      return leafs_.load();
    }

   private:
    exe::fibers::Routine MakeForker(size_t d) {
      return [this, d]() {
        if (d > 2) {
          fibers::Go(MakeForker(d - 2));
          fibers::Go(MakeForker(d - 1));
        } else {
          leafs_.fetch_add(1);
        }
      };
    }

   private:
    executors::ThreadPool pool_;
    std::atomic<size_t> leafs_{0};
  };

//  // Under investigation
//  TEST(Forks, wheels::test::TestOptions().TimeLimit(10s).AdaptTLToSanitizer()) {
//    ForkTester tester{4};
//    // Respect ThreadSanitizer thread limit:
//    // Tid - 13 bits => 8192 threads
//    ASSERT_EQ(tester.Explode(20), 6765);
//  }

  SIMPLE_TEST(TwoPools1) {
    executors::ThreadPool pool_1{4};
    executors::ThreadPool pool_2{4};

    auto make_tester = [](executors::ThreadPool& pool) {
      return [&pool]() {
        ExpectPool(pool);
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
    executors::ThreadPool pool_2{4};

    auto make_tester = [](executors::ThreadPool& pool) {
      return [&pool]() {
        static const size_t kIterations = 1024;

        for (size_t i = 0; i < kIterations; ++i) {
          ExpectPool(pool);

          fibers::self::Yield();

          fibers::Go(pool, [&pool]() {
            ExpectPool(pool);
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
