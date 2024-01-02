#pragma once

#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>
#include <twist/ed/stdlike/atomic.hpp>

#include <cstdlib>
#include <iostream>

namespace tp {

class WaitGroup {
 public:
  // += count
  void Add(size_t count) {
    std::unique_lock<twist::ed::stdlike::mutex> lock(mutex_);
    counter_.fetch_add(count);
  }

  // =- 1
  void Done() {
    std::unique_lock<twist::ed::stdlike::mutex> lock(mutex_);
    if(counter_.fetch_sub(1) == 1){
      is_zero.notify_all();
    }
  }

  // == 0
  // One-shot
  void Wait() {
    std::unique_lock<twist::ed::stdlike::mutex> lock(mutex_);
    while (counter_.load() > 0){
      is_zero.wait(lock);
    }
  }

 private:
  twist::ed::stdlike::atomic<int32_t> counter_{0};
  twist::ed::stdlike::condition_variable is_zero;
  twist::ed::stdlike::mutex mutex_;
};


}// namespace tp
