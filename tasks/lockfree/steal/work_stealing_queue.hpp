#pragma once

#include <array>
#include <span>

// Single-Producer / Multi-Consumer Bounded Ring Buffer

template <typename T, size_t Capacity>
class WorkStealingQueue {
  struct Slot {
    T* item;
  };

 public:
  bool TryPush(T* /*item*/) {
    return false;  // Not implemented
  }

  // Returns nullptr if queue is empty
  T* TryPop() {
    return nullptr;  // Not implemented
  }

  // Returns number of tasks
  size_t Grab(std::span<T*> /*out_buffer*/) {
    return 0;  // Not implemented
  }

 private:
  std::array<Slot, Capacity> buffer_;
};
