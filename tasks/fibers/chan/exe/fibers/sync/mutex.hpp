#pragma once

#include <twist/ed/stdlike/atomic.hpp>

namespace exe::fibers {

class Mutex {
 public:
  void Lock() {
    // Not implemented
  }

  void Unlock() {
    // Not implemented
  }

  // BasicLockable

  void lock() {  // NOLINT
    Lock();
  }

  void unlock() {  // NOLINT
    Unlock();
  }

 private:
  // ???
};

}  // namespace exe::fibers
