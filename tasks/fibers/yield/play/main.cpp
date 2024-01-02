#include <exe/tp/thread_pool.hpp>

#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sched/yield.hpp>

#include <fmt/core.h>

using namespace exe;

int main() {
  tp::ThreadPool scheduler{/*threads=*/4};
  scheduler.Start();

  for (size_t i = 0; i < 256; ++i) {
    fibers::Go(scheduler, [] {
      for (size_t j = 0; j < 3; ++j) {
        fibers::Yield();
      }
    });
  }

  scheduler.WaitIdle();
  scheduler.Stop();

  return 0;
}
