#include <exe/executors/thread_pool.hpp>
#include <exe/executors/strand.hpp>
#include <exe/executors/manual.hpp>
#include <exe/executors/submit.hpp>

#include <exe/threads/wait_group.hpp>

#include <wheels/test/framework.hpp>
#include <wheels/core/stop_watch.hpp>

#include <thread>
#include <deque>
#include <atomic>

using namespace exe;
using namespace std::chrono_literals;

void AssertRunningOn(executors::ThreadPool& pool) {
  ASSERT_TRUE(executors::ThreadPool::Current() == &pool);
}

TEST_SUITE(Strand) {
  SIMPLE_TEST(JustWorks) {
    executors::ThreadPool pool{4};
    pool.Start();

    executors::Strand strand{pool};

    threads::WaitGroup wg;
    wg.Add(1);

    executors::Submit(strand, [&] {
      wg.Done();
    });

    wg.Wait();

    pool.Stop();
  }

  SIMPLE_TEST(Decorator) {
    executors::ThreadPool pool{4};
    pool.Start();

    executors::Strand strand{pool};

    static const size_t kTasks = 128;

    threads::WaitGroup wg;
    wg.Add(kTasks);

    for (size_t i = 0; i < kTasks; ++i) {
      executors::Submit(strand, [&] {
        AssertRunningOn(pool);
        wg.Done();
      });
    }

    wg.Wait();

    pool.Stop();
  }

  SIMPLE_TEST(Counter) {
    executors::ThreadPool pool{13};
    pool.Start();

    executors::Strand strand{pool};

    static const size_t kIncrements = 1234;

    threads::WaitGroup wg;
    wg.Add(kIncrements);

    for (size_t i = 0; i < kIncrements; ++i) {
      executors::Submit(strand, [&] {
        AssertRunningOn(pool);
        wg.Done();
      });
    };

    wg.Wait();

    pool.Stop();
  }

  SIMPLE_TEST(Fifo) {
    executors::ThreadPool pool{13};
    pool.Start();

    executors::Strand strand{pool};

    static const size_t kTasks = 12345;

    size_t index = 0;

    threads::WaitGroup wg;
    wg.Add(kTasks);

    for (size_t i = 0; i < kTasks; ++i) {
      executors::Submit(strand, [&, me = i] {
        AssertRunningOn(pool);
        ASSERT_EQ(index++, me);
        wg.Done();
      });
    };

    wg.Wait();

    pool.Stop();
  }

  SIMPLE_TEST(ConcurrentStrands) {
    executors::ThreadPool pool{16};
    pool.Start();

    static const size_t kStrands = 50;

    std::deque<executors::Strand> strands;
    for (size_t i = 0; i < kStrands; ++i) {
      strands.emplace_back(pool);
    }

    static const size_t kPushes = 25;
    static const size_t kIterations = 25;

    threads::WaitGroup wg;
    wg.Add(kStrands * kPushes * kIterations);

    for (size_t i = 0; i < kIterations; ++i) {
      for (size_t j = 0; j < kStrands; ++j) {
        for (size_t k = 0; k < kPushes; ++k) {
          executors::Submit(strands[j], [&] {
            wg.Done();
          });
        }
      }
    }

    wg.Wait();

    pool.Stop();
  }

  SIMPLE_TEST(ConcurrentSubmits) {
    executors::ThreadPool workers{2};
    executors::Strand strand{workers};

    executors::ThreadPool clients{4};

    workers.Start();
    clients.Start();

    static const size_t kTasks = 1024;

    threads::WaitGroup wg;
    wg.Add(kTasks);

    for (size_t i = 0; i < kTasks; ++i) {
      executors::Submit(clients, [&] {
        executors::Submit(strand, [&] {
          AssertRunningOn(workers);
          wg.Done();
        });
      });
    }

    wg.Wait();

    workers.Stop();
    clients.Stop();
  }

  SIMPLE_TEST(StrandOverManual) {
    executors::ManualExecutor manual;
    executors::Strand strand{manual};

    static const size_t kTasks = 117;

    size_t tasks = 0;

    for (size_t i = 0; i < kTasks; ++i) {
      executors::Submit(strand, [&] {
        ++tasks;
      });
    }

    manual.Drain();

    ASSERT_EQ(tasks, kTasks);
  }

  SIMPLE_TEST(Batching) {
    executors::ManualExecutor manual;
    executors::Strand strand{manual};

    static const size_t kTasks = 100;

    size_t completed = 0;
    for (size_t i = 0; i < kTasks; ++i) {
      executors::Submit(strand, [&completed] {
        ++completed;
      });
    };

    size_t tasks = manual.Drain();
    ASSERT_LT(tasks, 5);
  }

  SIMPLE_TEST(StrandOverStrand) {
    executors::ThreadPool pool{4};
    pool.Start();

    executors::Strand strand_1{pool};
    executors::Strand strand_2{(executors::IExecutor&)strand_1};

    static const size_t kTasks = 17;

    threads::WaitGroup wg;
    wg.Add(kTasks);

    for (size_t i = 0; i < kTasks; ++i) {
      executors::Submit(strand_2, [&wg] {
        wg.Done();
      });
    }

    wg.Wait();

    pool.Stop();
  }

  SIMPLE_TEST(DoNotOccupyThread) {
    executors::ThreadPool pool{1};
    pool.Start();

    executors::Strand strand{pool};

    std::atomic<size_t> tasks{0};

    ++tasks;

    executors::Submit(pool, [&] {
      std::this_thread::sleep_for(1s);
      --tasks;
    });

    std::atomic<bool> stop_requested{false};

    auto snooze = [&tasks] {
      std::this_thread::sleep_for(10ms);
      --tasks;
    };

    for (size_t i = 0; i < 100; ++i) {
      ++tasks;
      executors::Submit(strand, snooze);
    }

    executors::Submit(pool, [&stop_requested] {
      stop_requested.store(true);
    });

    while (!stop_requested.load()) {
      ++tasks;
      executors::Submit(strand, snooze);
      std::this_thread::sleep_for(10ms);
    }

    // Wait idle
    while (tasks.load() > 0) {
      std::this_thread::sleep_for(1ms);
    }

    pool.Stop();
  }

  SIMPLE_TEST(NonBlockingSubmit) {
    executors::ThreadPool pool{1};
    executors::Strand strand{pool};

    pool.Start();

    threads::WaitGroup wg;
    wg.Add(2);

    executors::Submit(strand, [&] {
      // Bubble
      std::this_thread::sleep_for(3s);
      wg.Done();
    });

    std::this_thread::sleep_for(256ms);

    {
      wheels::StopWatch stop_watch;
      executors::Submit(strand, [&] {
        // Do nothing
        wg.Done();
      });

      ASSERT_LE(stop_watch.Elapsed(), 100ms);
    }

    wg.Wait();

    pool.Stop();
  }
}

RUN_ALL_TESTS()
