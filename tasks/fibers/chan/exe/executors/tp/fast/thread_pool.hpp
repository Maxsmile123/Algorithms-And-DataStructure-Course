#pragma once

#include <exe/executors/executor.hpp>

namespace exe::executors::tp::fast {

// Scalable work-stealing scheduler for short-lived tasks

class ThreadPool : public IExecutor {
  friend class Worker;

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
  // ???
};

}  // namespace exe::executors::tp::fast
