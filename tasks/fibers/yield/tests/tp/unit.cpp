#include <exe/tp/thread_pool.hpp>
#include <exe/tp/submit.hpp>

#include <wheels/test/framework.hpp>

#include <wheels/test/util/cpu_timer.hpp>
#include <wheels/core/stop_watch.hpp>

#include <atomic>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

using namespace exe;

TEST_SUITE(ThreadPool) {
  SIMPLE_TEST(JustWorks) {
    tp::ThreadPool pool{4};

    pool.Start();

    tp::Submit(pool, [] {
      std::cout << "Hello from thread pool!" << std::endl;
    });

    pool.WaitIdle();
    pool.Stop();
  }

  SIMPLE_TEST(Wait) {
    tp::ThreadPool pool{4};

    pool.Start();

    bool done = false;

    tp::Submit(pool, [&] {
      std::this_thread::sleep_for(1s);
      done = true;
    });

    pool.WaitIdle();
    pool.Stop();

    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(MultiWait) {
    tp::ThreadPool pool{1};

    pool.Start();

    for (size_t i = 0; i < 3; ++i) {
      bool done = false;

      tp::Submit(pool, [&] {
        std::this_thread::sleep_for(1s);
        done = true;
      });

      pool.WaitIdle();

      ASSERT_TRUE(done);
    }

    pool.Stop();
  }

  SIMPLE_TEST(ManyTasks) {
    tp::ThreadPool pool{4};

    pool.Start();

    static const size_t kTasks = 17;

    std::atomic<size_t> tasks{0};

    for (size_t i = 0; i < kTasks; ++i) {
      tp::Submit(pool, [&] {
        ++tasks;
      });
    }

    pool.WaitIdle();
    pool.Stop();

    ASSERT_EQ(tasks.load(), kTasks);
  }

  SIMPLE_TEST(Parallel) {
    tp::ThreadPool pool{4};

    pool.Start();

    std::atomic<size_t> tasks{0};

    tp::Submit(pool, [&] {
      std::this_thread::sleep_for(1s);
      ++tasks;
    });

    tp::Submit(pool, [&] {
      ++tasks;
    });

    std::this_thread::sleep_for(100ms);

    ASSERT_EQ(tasks.load(), 1);

    pool.WaitIdle();
    pool.Stop();

    ASSERT_EQ(tasks.load(), 2);
  }

  SIMPLE_TEST(TwoPools) {
    tp::ThreadPool pool1{1};
    tp::ThreadPool pool2{1};

    pool1.Start();
    pool2.Start();

    std::atomic<size_t> tasks{0};

    wheels::StopWatch stop_watch;

    tp::Submit(pool1, [&] {
      std::this_thread::sleep_for(1s);
      ++tasks;
    });

    tp::Submit(pool2, [&] {
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

  SIMPLE_TEST(Stop) {
    tp::ThreadPool pool{1};

    pool.Start();

    for (size_t i = 0; i < 3; ++i) {
      tp::Submit(pool, [] {
        std::this_thread::sleep_for(128ms);
      });
    }

    pool.Stop();
  }

  SIMPLE_TEST(DoNotBurnCPU) {
    tp::ThreadPool pool{4};

    pool.Start();

    // Warmup
    for (size_t i = 0; i < 4; ++i) {
      tp::Submit(pool, [&] {
        std::this_thread::sleep_for(100ms);
      });
    }

    wheels::ProcessCPUTimer cpu_timer;

    std::this_thread::sleep_for(1s);

    pool.WaitIdle();
    pool.Stop();

    ASSERT_TRUE(cpu_timer.Spent() < 100ms);
  }

  SIMPLE_TEST(Current) {
    tp::ThreadPool pool{1};

    pool.Start();

    ASSERT_EQ(tp::ThreadPool::Current(), nullptr);

    tp::Submit(pool, [&] {
      ASSERT_EQ(tp::ThreadPool::Current(), &pool);
    });

    pool.WaitIdle();
    pool.Stop();
  }

  SIMPLE_TEST(SubmitAfterWaitIdle) {
    tp::ThreadPool pool{4};

    pool.Start();

    bool done = false;

    tp::Submit(pool, [&] {
      std::this_thread::sleep_for(500ms);

      tp::Submit(*tp::ThreadPool::Current(), [&] {
        std::this_thread::sleep_for(500ms);
        done = true;
      });
    });

    pool.WaitIdle();
    pool.Stop();

    ASSERT_TRUE(done);
  }
  // TODO: Why 1,5 seconds? 
  TEST(UseThreads, wheels::test::TestOptions().TimeLimit(2s)) {
    tp::ThreadPool pool{4};

    pool.Start();

    std::atomic<size_t> tasks{0};

    for (size_t i = 0; i < 4; ++i) {
      tp::Submit(pool, [&] {
        std::this_thread::sleep_for(750ms);
        ++tasks;
      });
    }

    pool.WaitIdle();
    pool.Stop();

    ASSERT_EQ(tasks.load(), 4);
  }

  TEST(TooManyThreads, wheels::test::TestOptions().TimeLimit(2s)) {
    tp::ThreadPool pool{3};

    pool.Start();

    std::atomic<size_t> tasks{0};

    for (size_t i = 0; i < 4; ++i) {
      tp::Submit(pool, [&] {
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
      tp::Submit(*tp::ThreadPool::Current(), [] {
        KeepAlive();
      });
    }
  }

  TEST(KeepAlive, wheels::test::TestOptions().TimeLimit(4s)) {
    tp::ThreadPool pool{3};

    pool.Start();

    for (size_t i = 0; i < 5; ++i) {
      tp::Submit(pool, [] {
        KeepAlive();
      });
    }

    wheels::StopWatch stop_watch;

    pool.WaitIdle();
    pool.Stop();

    ASSERT_TRUE(stop_watch.Elapsed() > 3s);
  }

  SIMPLE_TEST(TaskLifetime) {
    tp::ThreadPool pool{4};

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
      tp::Submit(pool, Task(dead));
    }
    std::this_thread::sleep_for(500ms);
    ASSERT_EQ(dead.load(), 4)

    pool.WaitIdle();
    pool.Stop();
  }

  SIMPLE_TEST(Racy) {
    tp::ThreadPool pool{4};

    pool.Start();

    std::atomic<int> shared_counter{0};
    std::atomic<int> tasks{0};

    for (size_t i = 0; i < 100500; ++i) {
      tp::Submit(pool, [&] {
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
