#pragma once

#include <exe/executors/task.hpp>

#include <array>
#include <span>

namespace exe::executors::tp::fast {

// Single producer / multiple consumers bounded queue
// for local tasks

template <size_t Capacity>
class WorkStealingQueue {
  struct Slot {
    TaskBase* item;
  };

 public:
  // Single producer

  bool TryPush(TaskBase* /*item*/) {
    return false;  // Not implemented
  }

  // For grabbing from global queue / for stealing
  // Should always succeed
  void PushMany(std::span<TaskBase*> /*buffer*/) {
    // Not implemented
  }

  // Multiple consumers

  // Returns nullptr if queue is empty
  TaskBase* TryPop() {
    return nullptr;  // Not implemented
  }

  // For stealing and for offloading to global queue
  // Returns number of tasks in `out_buffer`
  size_t Grab(std::span<TaskBase*> /*out_buffer*/) {
    return 0;  // Not implemented
  }

 private:
  std::array<Slot, Capacity> buffer_;
};

}  // namespace exe::executors::tp::fast
