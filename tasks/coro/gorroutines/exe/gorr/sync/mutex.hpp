#pragma once

#include <twist/ed/stdlike/atomic.hpp>

#include <coroutine>

// std::unique_lock
#include <mutex>

namespace exe::gorr {

class Mutex {
  using UniqueLock = std::unique_lock<Mutex>;

  struct [[nodiscard]] Locker {
    Mutex& mutex_;

    explicit Locker(Mutex& mutex) : mutex_(mutex) {
    }

    // Awaiter protocol

    // NOLINTNEXTLINE
    bool await_ready() {
      return false;  // Not implemented
    }

    // NOLINTNEXTLINE
    void await_suspend(std::coroutine_handle<>) {
      // Not implemented
    }

    // NOLINTNEXTLINE
    UniqueLock await_resume() {
      std::abort();  // Not implemented
    }
  };

 public:
  // Asynchronous
  auto ScopedLock() {
    return Locker{*this};
  }

  bool TryLock() {
    return false;  // Not implemented
  }

  // For TryLock
  void Unlock() {
    // Not implemented
  }

  // For std::unique_lock
  // Do not use directly
  void unlock() {  // NOLINT
    Unlock();
  }

 private:
  // Returns true if lock acquired
  bool TryLockOrEnqueue(Locker* /*locker*/) {
    return false;  // Not implemented
  }

 private:
  // ???
};

}  // namespace exe::gorr
