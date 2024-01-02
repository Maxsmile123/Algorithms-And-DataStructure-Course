#pragma once

#include <exe/fibers/sched/go.hpp>

#include <asio/io_context.hpp>

#include <vector>
#include <thread>

template <typename F>
void RunScheduler(size_t threads, F init) {
  asio::io_context scheduler;

  bool done = false;

  // Spawn initial fiber
  exe::fibers::Go(scheduler, [init, &done]() {
    init();
    done = true;
  });

  std::vector<std::thread> workers;

  for (size_t i = 0; i < threads - 1; ++i) {
    workers.emplace_back([&scheduler] {
      scheduler.run();
    });
  }

  scheduler.run();

  // Join runners
  for (auto& t : workers) {
    t.join();
  }

  ASSERT_TRUE(done);
}
