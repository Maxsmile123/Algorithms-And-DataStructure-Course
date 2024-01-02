#pragma once

#include <exe/executors/task.hpp>

namespace exe::executors {

struct IExecutor {
  virtual ~IExecutor() = default;

  virtual void Execute(TaskBase* task) = 0;
};

}  // namespace exe::executors
