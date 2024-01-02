#pragma once

#include <exe/tp/queue.hpp>
#include <exe/tp/task.hpp>
#include <exe/threads/stdlike/wait_group.hpp>
#include <exe/threads/stdlike/mutex.hpp>

#include <twist/ed/stdlike/thread.hpp>

#include <twist/ed/local/ptr.hpp>

#include <vector>
#include <iostream>
#include <cassert>

namespace exe::tp {

// Fixed-size pool of worker threads

class ThreadPool {
 public:
  explicit ThreadPool(size_t threads);
  ~ThreadPool();

  // Non-copyable
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  // Non-movable
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  // Launches worker threads
  void Start();

  // Schedules task for execution in one of the worker threads
  void Submit(Task);

  // Locates current thread pool from worker thread
  static ThreadPool* Current();

  // Waits until outstanding work count reaches zero
  void WaitIdle();

  // Stops the worker threads as soon as possible
  void Stop();

 private:
  void Do();


 private:
  const size_t threads_number_;
  exe::threads::stdlike::Mutex mutex_;
  std::vector<twist::ed::stdlike::thread> workers_;
  UnboundedBlockingQueue<Task> task_queue_;
  exe::threads::stdlike::WaitGroup task_progress;
  twist::ed::stdlike::atomic<int32_t> is_stoped{0};

};

}  // namespace tp
