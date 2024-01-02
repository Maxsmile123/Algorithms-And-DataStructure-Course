#pragma once

#include <twist/ed/stdlike/atomic.hpp>

#include <cstdlib>

namespace exe::support {

// Test-and-TAS spinlock

class SpinLock {
 public:
  void Lock() {
    std::abort();  // Not implemented
  }

  void Unlock() {
    std::abort();  // Not implemented
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

}  // namespace exe::support
