#include <exe/executors/thread_pool.hpp>

#include <exe/gorr/run/fire.hpp>
#include <exe/gorr/run/teleport.hpp>
#include <exe/gorr/run/yield.hpp>

#include <exe/gorr/sync/mutex.hpp>
#include <exe/gorr/sync/wait_group.hpp>

#include <wheels/core/defer.hpp>

#include <iostream>

using namespace exe;

int main() {
  executors::ThreadPool scheduler{4};
  scheduler.Start();

  auto main = [&]() -> gorr::Task<> {
    co_await gorr::TeleportTo(scheduler);

    gorr::Mutex mutex;
    size_t cs = 0;

    gorr::WaitGroup wg;

    auto contender = [&]() -> gorr::Task<> {
      co_await gorr::TeleportTo(scheduler);

      wheels::Defer defer([&wg]() {
        wg.Done();
      });

      co_await gorr::Yield();

      for (size_t j = 0; j < 100'000; ++j) {
        auto lock = co_await mutex.ScopedLock();
        // Critical section
        ++cs;
      }
    };

    wg.Add(17);
    for (size_t i = 0; i < 17; ++i) {
      gorr::FireAndForget(contender());
    }

    co_await wg.Wait();

    std::cout << "# critical sections = " << cs << std::endl;
  };

  gorr::FireAndForget(main());

  scheduler.WaitIdle();

  scheduler.Stop();

  return 0;
}
