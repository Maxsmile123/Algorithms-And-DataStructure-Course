#include <exe/executors/tp/fast/thread_pool.hpp>

#include <exe/fibers/core/api.hpp>
#include <exe/fibers/sync/mutex.hpp>
#include <exe/fibers/sync/wait_group.hpp>
#include <exe/fibers/sync/channel.hpp>
#include <exe/fibers/sync/select.hpp>

#include <wheels/support/stop_watch.hpp>

#include <iostream>

using namespace exe;

//////////////////////////////////////////////////////////////////////

void WorkLoadChannels() {
  fibers::Channel<int> xs{7};
  fibers::Channel<int> ys{9};

  std::atomic<size_t> produced{0};
  std::atomic<size_t> consumed{0};

  fibers::WaitGroup wg;

  for (size_t k = 0; k < 50; ++k) {
    wg.Add(2);

    const size_t kSends = 100'500;

    // Producer
    fibers::Go([xs, ys, &produced, &wg]() mutable {
      for (size_t i = 0; i < kSends; ++i) {
        if (i % 2 == 0) {
          xs.Send(i);
        } else {
          ys.Send(i);
        }
        produced += i;
      }
      wg.Done();
    });

    // Consumer
    fibers::Go([xs, ys, &consumed, &wg]() mutable {
      for (size_t i = 0; i < kSends; ++i) {
        auto selected = fibers::Select(xs, ys);
        ;
        if (selected.index() == 0) {
          consumed += std::get<0>(selected);
        } else {
          consumed += std::get<1>(selected);
        }
      }
      wg.Done();
    });
  }

  wg.Wait();

  std::cout << produced << std::endl;
  std::cout << consumed << std::endl;
}

//////////////////////////////////////////////////////////////////////

void WorkLoadMutex() {
  for (size_t k = 0; k < 13; ++k) {
    fibers::Go([&]() {
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
              fibers::self::Yield();
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
    });
  }
}

//////////////////////////////////////////////////////////////////////

//void PrintMetrics(executors::tp::fast::PoolMetrics metrics) {
//  std::cout << "No metrics collected =(" << std::endl;
//}

void WorkLoad() {
  wheels::StopWatch sw;

  using Scheduler = executors::tp::fast::ThreadPool;

  Scheduler scheduler{4};

  fibers::Go(scheduler, []() {
    WorkLoadMutex();
  });

  scheduler.WaitIdle();
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
