#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/sys.hpp>

#include <cstdlib>

namespace stdlike {

class Mutex {
 public:
  void Lock() {
    uint32_t c;
    if (c = val.compare_exchange_strong(0, 1) != false){
      do{
        if (c == 2  || val.compare_exchange_strong(1, 2) != false){
          twist::ed::futex::Wait(val, 2);
        }
      } while(val.compare_exchange_strong(0, 2) != false);
    }
  }

  void Unlock() {
    auto wake_key = twist::ed::futex::PrepareWake(val);
    if (val.exchange(0) != true){
      twist::ed::futex::WakeOne(wake_key);
    }
  }

 private:
  // 0 - lock is free
  // 1 - lock is not free
  // 2 - lock is not free and exists threads that wait unlock
  twist::ed::stdlike::atomic<uint32_t> val{0};
};

}  // namespace stdlike
