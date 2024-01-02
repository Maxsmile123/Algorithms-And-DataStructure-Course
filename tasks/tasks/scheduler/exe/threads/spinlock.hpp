#pragma once

namespace exe::threads {

// Test-and-TAS spinlock

class SpinLock {
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

}  // namespace exe::threads
