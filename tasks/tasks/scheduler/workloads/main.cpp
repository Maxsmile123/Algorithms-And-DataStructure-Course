#include <exe/executors/tp/compute/thread_pool.hpp>
#include <exe/executors/tp/fast/thread_pool.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sched/yield.hpp>

#include <exe/fibers/sync/mutex.hpp>
#include <exe/fibers/sync/wait_group.hpp>

#include <exe/threads/wait_group.hpp>

#include <wheels/core/stop_watch.hpp>

#include <iostream>

using namespace exe;

//////////////////////////////////////////////////////////////////////

void WorkLoadYield() {
  fibers::WaitGroup wg;

  wg.Add(100'000);
  for (size_t i = 0; i < 100'000; ++i) {
    fibers::Go([&]() {
      for (size_t j = 0; j < 10; ++j) {
        fibers::Yield();
      }
      wg.Done();
    });
  }

  wg.Wait();
}

//////////////////////////////////////////////////////////////////////

void WorkLoadMutex() {
  fibers::WaitGroup clusters;

  clusters.Add(13);

  for (size_t k = 0; k < 13; ++k) {
    fibers::Go([&] {
      fibers::WaitGroup wg;

      size_t cs1 = 0;
      fibers::Mutex mutex1;

      size_t cs2 = 0;
      fibers::Mutex mutex2;

      wg.Add(100);

      for (size_t i = 0; i < 100; ++i) {
        fibers::Go([&]() {
          for (size_t j = 0; j < 65536; ++j) {
            {
              std::lock_guard g(mutex1);
              ++cs1;
            }
            if (j % 17 == 0) {
              fibers::Yield();
            }
            {
              std::lock_guard g(mutex2);
              ++cs2;
            }
          }

          wg.Done();
        });
      }

      wg.Wait();

      clusters.Done();
    });
  }

  clusters.Wait();
}

//////////////////////////////////////////////////////////////////////

void WorkLoadChannels() {
  // Not implemented
}

//////////////////////////////////////////////////////////////////////

void PrintMetrics(executors::tp::fast::PoolMetrics /*metrics*/) {
  std::cout << "No metrics =(" << std::endl;
}

void WorkLoad() {
  wheels::StopWatch sw;

  using Scheduler = executors::tp::fast::ThreadPool;

  Scheduler scheduler{4};
  scheduler.Start();

  threads::WaitGroup main;
  main.Add(1);

  fibers::Go(scheduler, [&main]() {
    WorkLoadYield();
    main.Done();
  });

  main.Wait();

  scheduler.Stop();

  const auto elapsed = sw.Elapsed();

  std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() << "ms " << std::endl;

  //PrintMetrics(scheduler.Metrics());
}

int main() {
  while (true) {
    WorkLoad();
  }
  return 0;
}
