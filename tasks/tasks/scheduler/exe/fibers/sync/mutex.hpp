#pragma once

// std::lock_guard and std::unique_lock
#include <mutex>

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
