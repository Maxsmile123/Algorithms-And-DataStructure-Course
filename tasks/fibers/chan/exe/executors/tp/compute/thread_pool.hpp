#pragma once

#include <exe/executors/executor.hpp>

#include <cstdlib>

namespace exe::executors::tp::compute {

// Thread pool for independent CPU-bound tasks
// Fixed pool of worker threads + shared unbounded blocking queue

class ThreadPool : public IExecutor {
 public:
  explicit ThreadPool(size_t threads);
  ~ThreadPool();

  // Non-copyable
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  // IExecutor
  void Execute(TaskBase* task) override;

  void WaitIdle();

  void Stop();

  static ThreadPool* Current();

 private:
  // Worker threads, task queue, etc
};

}  // namespace exe::executors::tp::compute
