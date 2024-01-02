#pragma once

#include <exe/executors/executor.hpp>

namespace exe::executors {

// Single-threaded task queue

class ManualExecutor : public IExecutor {
 public:
  // IExecutor
  void Execute(TaskBase* task) override;

  // Run tasks

  // Run at most `limit` tasks from queue
  // Returns number of completed tasks
  size_t RunAtMost(size_t limit);

  // Run next task if queue is not empty
  bool RunNext() {
    return RunAtMost(1) == 1;
  }

  // Run tasks until queue is empty
  // Returns number of completed tasks
  // Post-condition: HasTasks() == false
  size_t Drain();

  size_t TaskCount() const {
    return 0;  // Not implemented
  }

  bool HasTasks() const {
    return false;  // Not implemented
  }

 private:
  // ???
};

}  // namespace exe::executors
