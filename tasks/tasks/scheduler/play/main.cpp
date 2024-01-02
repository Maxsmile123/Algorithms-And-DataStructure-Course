#include <exe/executors/thread_pool.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sched/yield.hpp>

#include <exe/fibers/sync/mutex.hpp>
#include <exe/fibers/sync/wait_group.hpp>

#include <exe/threads/wait_group.hpp>

#include <fmt/core.h>

using namespace exe;

int main() {
  executors::ThreadPool scheduler{/*threads=*/4};
  scheduler.Start();

  threads::WaitGroup main;
  main.Add(1);

  fibers::Go(scheduler, [&main] {
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

    main.Done();
  });

  main.Wait();

  scheduler.Stop();

  return 0;
}
