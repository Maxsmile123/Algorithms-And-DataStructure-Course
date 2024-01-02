#include <exe/executors/thread_pool.hpp>
#include <exe/executors/execute.hpp>

#include <wheels/test/framework.hpp>
#include <twist/test/budget.hpp>

#include <course/twist/test.hpp>
#include <twist/test/runs.hpp>
#include <twist/test/race.hpp>

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/stdlike/thread.hpp>

#include <atomic>

////////////////////////////////////////////////////////////////////////////////

using exe::executors::ThreadPool;
using exe::executors::Execute;

////////////////////////////////////////////////////////////////////////////////

namespace tasks {

void KeepAlive() {
  if (twist::test::KeepRunning()) {
    Execute(*ThreadPool::Current(), []() {
      KeepAlive();
    });
  }
}

void Backoff() {
  twist::strand::stdlike::this_thread::yield();
}

void Test(size_t threads, size_t clients, size_t limit) {
  ThreadPool pool{threads};

  Execute(pool, []() {
    KeepAlive();
  });

  std::atomic<size_t> completed{0};

  twist::ed::stdlike::atomic<size_t> queue{0};

  twist::test::Race race;

  for (size_t i = 0; i < clients; ++i) {
    race.Add([&]() {
      while (twist::test::KeepRunning()) {
        // TrySubmit
        if (++queue <= limit) {
          Execute(pool, [&]() {
            --queue;
            ++completed;
          });
        } else {
          --queue;
          Backoff();
        }
      }
    });
  }

  race.Run();

  pool.WaitIdle();
  pool.Stop();

  std::cout << "Tasks completed: " << completed.load() << std::endl;

  ASSERT_EQ(queue.load(), 0);
  ASSERT_GT(completed.load(), 8888);
}

}  // namespace tasks

TWIST_TEST_TEMPLATE(Submits, gorr::Test)
->TimeLimit(4s)
->Run(3, 5, 111)
->Run(4, 3, 13)
->Run(2, 4, 5)
->Run(9, 10, 33);

////////////////////////////////////////////////////////////////////////////////

namespace wait_idle {

void TestOneTask() {
  ThreadPool pool{4};

  while (twist::test::KeepRunning()) {
    size_t tasks = 0;

    Execute(pool, [&]() {
      ++tasks;
    });

    pool.WaitIdle();

    ASSERT_EQ(tasks, 1);
  }

  pool.Stop();
}

void TestSeries() {
  ThreadPool pool{1};

  size_t iter = 0;

  while (twist::test::KeepRunning()) {
    ++iter;
    const size_t tasks = 1 + iter % 3;

    size_t tasks_completed = 0;
    for (size_t i = 0; i < tasks; ++i) {
      Execute(pool, [&](){
        ++tasks_completed;
      });
    }

    pool.WaitIdle();

    ASSERT_EQ(tasks_completed, tasks);
  }

  pool.Stop();
}

void TestCurrent() {
  ThreadPool pool{2};

  while (twist::test::KeepRunning()) {
    bool done = false;

    Execute(pool, [&]() {
      Execute(*ThreadPool::Current(), [&]() {
        done = true;
      });
    });

    pool.WaitIdle();

    ASSERT_TRUE(done);
  }

  pool.Stop();
}

void TestConcurrent() {
  ThreadPool pool{2};

  std::atomic<size_t> tasks = 0;

  twist::ed::stdlike::thread t1([&]() {
    Execute(pool, [&]() {
      ++tasks;
    });
  });

  twist::ed::stdlike::thread t2([&]() {
    pool.WaitIdle();
  });

  t1.join();
  t2.join();

  ASSERT_TRUE(tasks <= 1);

  pool.Stop();
}

}  // namespace wait_idle

TEST_SUITE(WaitIdle) {
  TWIST_TEST(OneTask, 5s) {
    wait_idle::TestOneTask();
  }

  TWIST_TEST(Series, 5s) {
    wait_idle::TestSeries();
  }

  TWIST_TEST(Current, 5s) {
    wait_idle::TestCurrent();
  }

  TWIST_TEST_REPEAT(Concurrent, 5s) {
    wait_idle::TestConcurrent();
  }
}

RUN_ALL_TESTS()
