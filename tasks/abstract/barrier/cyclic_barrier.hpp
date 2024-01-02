#pragma once

#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>

#include <cstdlib>
#include <iostream>

class CyclicBarrier {
 public:
  explicit CyclicBarrier(size_t participants): cap(participants), calls(0) {}

  void ArriveAndWait() {
    std::unique_lock<twist::ed::stdlike::mutex> lock(mutex_);
    size_t cur_iter = iteration;
    ++calls;
    if (calls == cap){
      calls = 0;
      ++iteration;
      no_zero.notify_all();
    } else{
      while (cur_iter == iteration){
        no_zero.wait(lock);
      }
    }
  }

 private:
  const size_t cap;
  size_t calls;
  size_t iteration = 0;
  twist::ed::stdlike::mutex mutex_;
  twist::ed::stdlike::condition_variable no_zero;
};
