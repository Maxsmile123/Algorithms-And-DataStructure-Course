#pragma once

#include <exe/executors/task.hpp>

#include <wheels/intrusive/forward_list.hpp>

#include <span>

namespace exe::executors::tp::fast {

// Unbounded queue shared between workers

class GlobalQueue {
 public:
  void Push(TaskBase* /*item*/) {
    // Not implemented
  }

  void Offload(std::span<TaskBase*> /*buffer*/) {
    // Not implemented
  }

  // Returns nullptr if queue is empty
  TaskBase* TryPop() {
    return nullptr;  // Not implemented
  }

  // Returns number of items in `out_buffer`
  size_t Grab(std::span<TaskBase*> /*out_buffer*/, size_t /*workers*/) {
    return 0;  // Not implemented
  }

 private:
  // ???
};

}  // namespace exe::executors::tp::fast
