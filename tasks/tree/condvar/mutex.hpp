#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/sys.hpp>

#include <cstdlib>

namespace stdlike {

class Mutex {
 public:
  void Lock() {
    thread_counter_.fetch_add(1);
    while(locked_.exchange(1) != 0){
      twist::ed::futex::Wait(locked_, 1);
    }
  }

  void Unlock() {
    auto wake_key = twist::ed::futex::PrepareWake(locked_);
    locked_.store(0);
    if (thread_counter_.fetch_sub(1) != 1){
      twist::ed::futex::WakeOne(wake_key);
    }
  }
  void lock(){
    Lock();
  }

  void unlock(){
    Unlock();
  }

 private:
  twist::ed::stdlike::atomic<uint32_t> locked_{0};
  twist::ed::stdlike::atomic<uint32_t> thread_counter_{0};
};
}