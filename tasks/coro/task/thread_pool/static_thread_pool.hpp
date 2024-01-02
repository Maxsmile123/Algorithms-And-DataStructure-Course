#pragma once

#include <function2/function2.hpp>

#include <asio.hpp>

#include <vector>
#include <thread>

namespace tp {

// Fixed-size pool of worker threads

class StaticThreadPool {
  using Task = fu2::unique_function<void()>;
 public:
  explicit StaticThreadPool(size_t threads);
  ~StaticThreadPool();

  // Non-copyable
  StaticThreadPool(const StaticThreadPool&) = delete;
  StaticThreadPool& operator=(const StaticThreadPool&) = delete;

  // Submit task for execution
  // Submitted task will be scheduled to run in one of the worker threads
  void Submit(Task task);

  // Locate current thread pool from running task / worker thread
  static StaticThreadPool* Current();

  // Graceful shutdown
  // Waits until outstanding work count has reached 0
  // and joins worker threads
  void Join();

 private:
  void StartWorkerThreads(size_t thread_count);

  // Worker routine
  void Work();

 private:
  // Use io_context as task queue
  asio::io_context io_context_;
  // Some magic for graceful shutdown
  asio::executor_work_guard<asio::io_context::executor_type> work_guard_;
  // Worker threads
  std::vector<std::thread> workers_;
  bool joined_{false};
};

inline StaticThreadPool* Current() {
  return StaticThreadPool::Current();
}

}  // namespace tp
