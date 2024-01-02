#pragma once

#include <exe/threads/stdlike/mutex.hpp>
#include <exe/threads/stdlike/condvar.hpp>
#include <twist/ed/stdlike/atomic.hpp>

#include <cstdlib>
#include <iostream>

namespace exe::threads::stdlike {

class WaitGroup {
 public:
  // += count
  void Add(size_t count) {
    std::unique_lock<Mutex> lock(mutex_);
    counter_.fetch_add(count);
  }

  // =- 1
  void Done() {
    std::unique_lock<Mutex> lock(mutex_);
    if(counter_.fetch_sub(1) == 1){
      is_zero.notify_all();
    }
  }

  // == 0
  // One-shot
  void Wait() {
    std::unique_lock<Mutex> lock(mutex_);
    while (counter_.load() > 0){
      is_zero.wait(lock);
    }
  }

 private:
  twist::ed::stdlike::atomic<int32_t> counter_{0};
  CondVar is_zero;
  Mutex mutex_;
};


}// namespace tp
