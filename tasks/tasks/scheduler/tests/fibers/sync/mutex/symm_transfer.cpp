#include <wheels/test/framework.hpp>

#include <exe/executors/thread_pool.hpp>
#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sync/mutex.hpp>
#include <exe/threads/wait_group.hpp>

#include <chrono>
#include <thread>

using namespace exe;

using namespace std::chrono_literals;

TEST_SUITE(MutexExtra) {
  SIMPLE_TEST(UnlockFork) {
    executors::ThreadPool scheduler{4};
    scheduler.Start();

    fibers::Mutex mutex;

    threads::WaitGroup wg;
    wg.Add(4);

    fibers::Go(scheduler, [&] {
      mutex.Lock();
      std::this_thread::sleep_for(1s);
      mutex.Unlock();

      wg.Done();
    });

    std::this_thread::sleep_for(128ms);

    for (size_t i = 0; i < 3; ++i) {
      fibers::Go(scheduler, [&]() {
        mutex.Lock();
        mutex.Unlock();
        std::this_thread::sleep_for(5s);

        wg.Done();
      });
    }

    wg.Wait();

    scheduler.Stop();
  }
}

RUN_ALL_TESTS()
