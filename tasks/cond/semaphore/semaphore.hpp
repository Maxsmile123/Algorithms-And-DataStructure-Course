#pragma once

#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>

#include <cstdlib>
#include <iostream>

class Semaphore {
 public:
  explicit Semaphore(size_t tokens): counter_(tokens) {}

  void Acquire() {
    std::unique_lock<twist::ed::stdlike::mutex> lock(mutex_);
    ++counter_;
    while(counter_ > 0){
      more_then_zero_.wait(lock);
    }
  }

  void Release() {
    std::lock_guard<twist::ed::stdlike::mutex> lock(mutex_);
    --counter_;
    more_then_zero_.notify_one();
  }

 private:
  size_t counter_ = 0;
  twist::ed::stdlike::condition_variable more_then_zero_;
  twist::ed::stdlike::mutex mutex_;
};
