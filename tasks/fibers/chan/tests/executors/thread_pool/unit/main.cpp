#include <exe/executors/thread_pool.hpp>
#include <exe/executors/execute.hpp>

#include <wheels/test/framework.hpp>

#include <wheels/support/cpu_time.hpp>

#include <atomic>
#include <chrono>
#include <thread>

using exe::executors::ThreadPool;
using exe::executors::Execute;

using namespace std::chrono_literals;

TEST_SUITE(ThreadPool) {
  SIMPLE_TEST(JustWorks) {
    ThreadPool pool{4};

    Execute(pool, []() {
      std::cout << "Hello from thread pool!" << std::endl;
    });

    pool.WaitIdle();
    pool.Stop();
  }

  SIMPLE_TEST(Wait) {
    ThreadPool pool{4};

    bool done = false;

    Execute(pool, [&]() {
      std::this_thread::sleep_for(1s);
      done = true;
    });

    pool.WaitIdle();
    pool.Stop();

    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(MultiWait) {
    ThreadPool pool{1};

    for (size_t i = 0; i < 3; ++i) {
      bool done = false;

      Execute(pool, [&]() {
        std::this_thread::sleep_for(1s);
        done = true;
      });

      pool.WaitIdle();
      ASSERT_TRUE(done);
    }

    pool.Stop();
  }

  SIMPLE_TEST(Exceptions) {
    ThreadPool pool{1};

    Execute(pool, []() {
      throw std::runtime_error("Task failed");
    });

    pool.WaitIdle();
    pool.Stop();
  }

  SIMPLE_TEST(ManyTasks) {
    ThreadPool pool{4};

    static const size_t kTasks = 17;

    std::atomic<size_t> tasks{0};

    for (size_t i = 0; i < kTasks; ++i) {
      Execute(pool, [&]() {
        ++tasks;
      });
    }

    pool.WaitIdle();
    pool.Stop();

    ASSERT_EQ(tasks.load(), kTasks);
  }

  SIMPLE_TEST(Parallel) {
    ThreadPool pool{4};

    std::atomic<size_t> tasks{0};

    Execute(pool, [&]() {
      std::this_thread::sleep_for(1s);
      ++tasks;
    });

    Execute(pool, [&]() {
      ++tasks;
    });

    std::this_thread::sleep_for(100ms);

    ASSERT_EQ(tasks.load(), 1);

    pool.WaitIdle();
    pool.Stop();

    ASSERT_EQ(tasks.load(), 2);
  }

  SIMPLE_TEST(TwoPools) {
    ThreadPool pool1{1};
    ThreadPool pool2{1};

    std::atomic<size_t> tasks{0};

    wheels::StopWatch stop_watch;

    Execute(pool1, [&]() {
      std::this_thread::sleep_for(1s);
      ++tasks;
    });

    Execute(pool2, [&]() {
      std::this_thread::sleep_for(1s);
      ++tasks;
    });

    pool2.WaitIdle();
    pool2.Stop();

    pool1.WaitIdle();
    pool1.Stop();

    ASSERT_TRUE(stop_watch.Elapsed() < 1500ms);
    ASSERT_EQ(tasks.load(), 2);
  }

  SIMPLE_TEST(CrossSubmit) {
    ThreadPool pool1{1};
    ThreadPool pool2{1};

    bool done = false;

    Execute(pool1, [&]() {
      ASSERT_TRUE(ThreadPool::Current() == &pool1);
      Execute(pool2, [&]() {
        ASSERT_TRUE(ThreadPool::Current() == &pool2);
        done = true;
      });
    });

    pool1.WaitIdle();
    pool2.WaitIdle();

    ASSERT_TRUE(done);

    pool1.Stop();
    pool2.Stop();
  }

  SIMPLE_TEST(Shutdown) {
    ThreadPool pool{3};

    for (size_t i = 0; i < 10; ++i) {
      Execute(pool, []() {
        std::this_thread::sleep_for(1s);
      });
    }

    std::this_thread::sleep_for(256ms);

    for (size_t i = 0; i < 100; ++i) {
      Execute(pool, []() {
        std::this_thread::sleep_for(1s);
      });
    }

    std::this_thread::sleep_for(256ms);

    pool.Stop();
  }

  SIMPLE_TEST(DoNotBurnCPU) {
    ThreadPool pool{4};

    // Warmup
    for (size_t i = 0; i < 4; ++i) {
      Execute(pool, [&]() {
        std::this_thread::sleep_for(100ms);
      });
    }

    wheels::ProcessCPUTimer cpu_timer;

    std::this_thread::sleep_for(1s);

    pool.WaitIdle();
    pool.Stop();

    ASSERT_TRUE(cpu_timer.Elapsed() < 100ms);
  }

  SIMPLE_TEST(Current) {
    ThreadPool pool{1};

    ASSERT_EQ(ThreadPool::Current(), nullptr);

    Execute(pool, [&]() {
      ASSERT_EQ(ThreadPool::Current(), &pool);
    });

    pool.WaitIdle();
    pool.Stop();
  }

  SIMPLE_TEST(ExecuteAfterWait) {
    ThreadPool pool{4};

    bool done = false;

    Execute(pool, [&]() {
      std::this_thread::sleep_for(500ms);
      Execute(*ThreadPool::Current(), [&]() {
        std::this_thread::sleep_for(500ms);
        done = true;
      });
    });

    pool.WaitIdle();
    pool.Stop();

    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(ExecuteAfterShutdown) {
    ThreadPool pool{4};

    bool done = false;

    Execute(pool, [&]() {
      std::this_thread::sleep_for(500ms);
      Execute(*ThreadPool::Current(), [&]() {
        std::this_thread::sleep_for(500ms);
        done = true;
      });
    });

    pool.Stop();

    ASSERT_FALSE(done);
  }

  TEST(UseThreads, wheels::test::TestOptions().TimeLimit(1s)) {
    ThreadPool pool{4};

    std::atomic<size_t> tasks{0};

    for (size_t i = 0; i < 4; ++i) {
      Execute(pool, [&]() {
        std::this_thread::sleep_for(750ms);
        ++tasks;
      });
    }

    pool.WaitIdle();
    pool.Stop();

    ASSERT_EQ(tasks.load(), 4);
  }

  TEST(TooManyThreads, wheels::test::TestOptions().TimeLimit(2s)) {
    ThreadPool pool{3};

    std::atomic<size_t> tasks{0};

    for (size_t i = 0; i < 4; ++i) {
      Execute(pool, [&]() {
        std::this_thread::sleep_for(750ms);
        ++tasks;
      });
    }

    wheels::StopWatch stop_watch;

    pool.WaitIdle();
    pool.Stop();

    ASSERT_TRUE(stop_watch.Elapsed() > 1s);
    ASSERT_EQ(tasks.load(), 4);
  }

  void KeepAlive() {
    if (wheels::test::TestTimeLeft() > 300ms) {
      Execute(*ThreadPool::Current(), []() {
        KeepAlive();
      });
    }
  }

  TEST(KeepAlive, wheels::test::TestOptions().TimeLimit(4s)) {
    ThreadPool pool{3};

    for (size_t i = 0; i < 5; ++i) {
      Execute(pool, []() {
        KeepAlive();
      });
    }

    wheels::StopWatch stop_watch;

    pool.WaitIdle();
    pool.Stop();

    ASSERT_TRUE(stop_watch.Elapsed() > 3s);
  }

  SIMPLE_TEST(TaskLifetime) {
    ThreadPool pool{4};

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
      Execute(pool, Task(dead));
    }
    std::this_thread::sleep_for(500ms);
    ASSERT_EQ(dead.load(), 4)

    pool.WaitIdle();
    pool.Stop();
  }

  SIMPLE_TEST(Racy) {
    ThreadPool pool{4};

    std::atomic<int> shared_counter{0};
    std::atomic<int> tasks{0};

    for (size_t i = 0; i < 100500; ++i) {
      Execute(pool, [&]() {
        int old = shared_counter.load();
        shared_counter.store(old + 1);

        ++tasks;
      });
    }

    pool.WaitIdle();
    pool.Stop();

    std::cout << "Racy counter value: " << shared_counter << std::endl;

    ASSERT_EQ(tasks.load(), 100500);
    ASSERT_LE(shared_counter.load(), 100500);
  }
}

RUN_ALL_TESTS()
