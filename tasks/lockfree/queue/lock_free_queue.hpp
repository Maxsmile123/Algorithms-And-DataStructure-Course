#pragma once

#include <twist/ed/stdlike/atomic.hpp>

#include <hazard/manager.hpp>

#include <optional>

// Michael-Scott unbounded MPMC lock-free queue

template <typename T>
class LockFreeQueue {
  struct Node {
    // Not implemented
  };

 public:
  explicit LockFreeQueue(hazard::Manager* gc)
      : gc_(gc) {
  }

  LockFreeQueue()
      : LockFreeQueue(hazard::Manager::Get()) {
  }

  void Push(T /*item*/) {
    // Not implemented
  }

  std::optional<T> TryPop() {
    // Not implemented
  }

  ~LockFreeQueue() {
    // Not implemented
  }

 private:
  hazard::Manager* gc_;
};
