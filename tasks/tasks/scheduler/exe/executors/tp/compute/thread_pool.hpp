#pragma once

#include <exe/executors/executor.hpp>

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

  // Non-movable
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  void Start();

  // IExecutor
  void Submit(TaskBase*) override;

  static ThreadPool* Current();

  void Stop();

 private:
  // ???
};

}  // namespace exe::executors::tp::compute
