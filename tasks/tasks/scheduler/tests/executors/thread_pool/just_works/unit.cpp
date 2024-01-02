#include <exe/executors/thread_pool.hpp>
#include <exe/executors/submit.hpp>

#include <exe/threads/wait_group.hpp>

#include <wheels/test/framework.hpp>

#include <wheels/test/util/cpu_timer.hpp>
#include <wheels/core/stop_watch.hpp>

#include <atomic>
#include <chrono>
#include <thread>

using namespace exe;

using namespace std::chrono_literals;

TEST_SUITE(ThreadPool) {
  SIMPLE_TEST(JustWorks) {
    executors::ThreadPool pool{4};

    pool.Start();

    threads::WaitGroup wg;
    wg.Add(1);

    executors::Submit(pool, [&wg] {
      std::cout << "Hello from thread pool!" << std::endl;
      wg.Done();
    });

    wg.Wait();

    pool.Stop();
  }

  SIMPLE_TEST(Wait) {
    executors::ThreadPool pool{4};

    pool.Start();

    threads::WaitGroup wg;
    wg.Add(1);

    executors::Submit(pool, [&] {
      std::this_thread::sleep_for(1s);
      wg.Done();
    });

    wg.Wait();

    pool.Stop();
  }

  SIMPLE_TEST(MultiWait) {
    executors::ThreadPool pool{1};

    pool.Start();

    for (size_t i = 0; i < 3; ++i) {
      threads::WaitGroup wg;
      wg.Add(1);

      executors::Submit(pool, [&] {
        std::this_thread::sleep_for(1s);
        wg.Done();
      });

      wg.Wait();
    }

    pool.Stop();
  }

  SIMPLE_TEST(ManyTasks) {
    executors::ThreadPool pool{4};

    pool.Start();

    static const size_t kTasks = 17;

    threads::WaitGroup wg;
    wg.Add(kTasks);

    for (size_t i = 0; i < kTasks; ++i) {
      executors::Submit(pool, [&] {
        wg.Done();
      });
    }

    wg.Wait();

    pool.Stop();
  }

//  TEST(UseThreads, wheels::test::TestOptions().TimeLimit(1s)) {
//    executors::ThreadPool pool{4};
//    pool.Start();
//
//    threads::WaitGroup wg;
//    wg.Add(4);
//
//    for (size_t i = 0; i < 4; ++i) {
//      executors::Submit(pool, [&] {
//        std::this_thread::sleep_for(750ms);
//        wg.Done();
//      });
//    }
//
//    wg.Wait();
//
//    pool.Stop();
//  }

  TEST(TooManyThreads, wheels::test::TestOptions().TimeLimit(2s)) {
    executors::ThreadPool pool{3};

    pool.Start();

    threads::WaitGroup wg;
    wg.Add(4);

    for (size_t i = 0; i < 4; ++i) {
      executors::Submit(pool, [&] {
        std::this_thread::sleep_for(750ms);
        wg.Done();
      });
    }

    wheels::StopWatch stop_watch;

    wg.Wait();

    ASSERT_TRUE(stop_watch.Elapsed() > 1s);

    pool.Stop();
  }

  SIMPLE_TEST(TwoPools) {
    executors::ThreadPool pool1{1};
    executors::ThreadPool pool2{1};

    pool1.Start();
    pool2.Start();

    threads::WaitGroup wg;
    wg.Add(2);

    wheels::StopWatch stop_watch;

    executors::Submit(pool1, [&] {
      std::this_thread::sleep_for(1s);
      wg.Done();
    });

    executors::Submit(pool2, [&] {
      std::this_thread::sleep_for(1s);
      wg.Done();
    });

    wg.Wait();

    ASSERT_TRUE(stop_watch.Elapsed() < 1500ms);

    pool2.Stop();
    pool1.Stop();
  }

  SIMPLE_TEST(CrossSubmit) {
    executors::ThreadPool pool1{1};
    executors::ThreadPool pool2{1};

    pool1.Start();
    pool2.Start();

    threads::WaitGroup wg;
    wg.Add(1);

    executors::Submit(pool1, [&]() {
      ASSERT_TRUE(executors::ThreadPool::Current() == &pool1);
      executors::Submit(pool2, [&]() {
        ASSERT_TRUE(executors::ThreadPool::Current() == &pool2);
        wg.Done();
      });
    });

    wg.Wait();

    pool1.Stop();
    pool2.Stop();
  }

  SIMPLE_TEST(DoNotBurnCPU) {
    executors::ThreadPool pool{4};

    pool.Start();

    threads::WaitGroup wg;
    wg.Add(4);

    wheels::ProcessCPUTimer cpu_timer;

    // Warmup
    for (size_t i = 0; i < 4; ++i) {
      executors::Submit(pool, [&] {
        std::this_thread::sleep_for(100ms);
        wg.Done();
      });
    }

    wg.Wait();

    pool.Stop();

    ASSERT_TRUE(cpu_timer.Spent() < 100ms);
  }

  SIMPLE_TEST(Current) {
    executors::ThreadPool pool{1};

    pool.Start();

    ASSERT_EQ(executors::ThreadPool::Current(), nullptr);

    threads::WaitGroup wg;
    wg.Add(1);

    executors::Submit(pool, [&] {
      ASSERT_EQ(executors::ThreadPool::Current(), &pool);
      wg.Done();
    });

    wg.Wait();

    pool.Stop();
  }

  SIMPLE_TEST(SubmitAfterWait) {
    executors::ThreadPool pool{4};

    pool.Start();

    threads::WaitGroup wg;
    wg.Add(1);

    executors::Submit(pool, [&] {
      std::this_thread::sleep_for(500ms);

      executors::Submit(*executors::ThreadPool::Current(), [&] {
        std::this_thread::sleep_for(500ms);
        wg.Done();
      });
    });

    wg.Wait();

    pool.Stop();
  }

  SIMPLE_TEST(TaskLifetime) {
    executors::ThreadPool pool{4};

    pool.Start();

    std::atomic<int> dead{0};

    class Task {
     public:
      Task(std::atomic<int>& done) : counter_(done) {
      }
      Task(const Task&) = delete;
      Task(Task&&) = default;

      ~Task() {
        if (done_) {
          counter_.fetch_add(1);
        }
      }

      void operator()() {
        std::this_thread::sleep_for(100ms);
        done_ = true;
      }

     private:
      bool done_{false};
      std::atomic<int>& counter_;
    };

    for (int i = 0; i < 4; ++i) {
      executors::Submit(pool, Task(dead));
    }
    std::this_thread::sleep_for(1s);
    ASSERT_EQ(dead.load(), 4)

    pool.Stop();
  }

  SIMPLE_TEST(Racy) {
    executors::ThreadPool pool{4};
    pool.Start();

    static const size_t kTasks = 100500;

    std::atomic<size_t> shared_counter{0};

    threads::WaitGroup wg;
    wg.Add(kTasks);

    for (size_t i = 0; i < kTasks; ++i) {
      executors::Submit(pool, [&] {
        int old = shared_counter.load();
        shared_counter.store(old + 1);

        wg.Done();
      });
    }

    wg.Wait();

    std::cout << "Racy counter value: " << shared_counter << std::endl;

    ASSERT_LE(shared_counter.load(), kTasks);

    pool.Stop();
  }
}

RUN_ALL_TESTS()
