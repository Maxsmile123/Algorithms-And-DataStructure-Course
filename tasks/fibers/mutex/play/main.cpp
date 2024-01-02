#include <exe/executors/thread_pool.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sched/yield.hpp>

#include <exe/fibers/sync/mutex.hpp>
#include <exe/fibers/sync/wait_group.hpp>

#include <fmt/core.h>

using namespace exe;

int main() {
  tp::ThreadPool scheduler{/*threads=*/4};
  scheduler.Start();

  fibers::Go(scheduler, [] {
    fibers::WaitGroup wg;

    fibers::Mutex mutex;
    size_t cs = 0;

    wg.Add(3);

    for (size_t i = 0; i < 3; ++i) {
      fibers::Go([&] {
        for (size_t j = 0; j < 1024; ++j) {
          std::lock_guard guard(mutex);
          ++cs;
        }
        wg.Done();
      });
    }

    wg.Wait();

    fmt::println("# critical sections: {}", cs);
  });

  scheduler.WaitIdle();
  scheduler.Stop();

  return 0;
}
