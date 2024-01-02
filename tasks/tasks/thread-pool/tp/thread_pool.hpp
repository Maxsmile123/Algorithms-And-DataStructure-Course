#pragma once

#include <tp/queue.hpp>
#include <tp/task.hpp>
#include <tp/wait_group.hpp>

#include <twist/ed/stdlike/thread.hpp>
#include <twist/ed/stdlike/mutex.hpp>

#include <twist/ed/local/ptr.hpp>

#include <vector>
#include <iostream>
#include <cassert>

namespace tp {

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
  twist::ed::stdlike::mutex mutex_;
  std::vector<twist::ed::stdlike::thread> workers_;
  UnboundedBlockingQueue<Task> task_queue_;
  WaitGroup task_progress;
  twist::ed::stdlike::atomic<int32_t> is_stoped{0};

};

}  // namespace tp
