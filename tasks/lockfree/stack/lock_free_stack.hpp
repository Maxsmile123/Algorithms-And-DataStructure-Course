#pragma once

#include "atomic_stamped_ptr.hpp"

#include <twist/ed/stdlike/atomic.hpp>

#include <optional>

// Treiber unbounded lock-free stack
// https://en.wikipedia.org/wiki/Treiber_stack

template <typename T>
class LockFreeStack {
  struct Node {
    T value;
    // ???
  };

 public:
  void Push(T /*item*/) {
    // Not implemented
  }

  std::optional<T> TryPop() {
    return std::nullopt;  // Not implemented
  }

  ~LockFreeStack() {
    // Not implemented
  }

 private:
  // ???
};
