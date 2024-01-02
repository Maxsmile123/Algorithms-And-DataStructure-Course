#pragma once

#include <exe/executors/task.hpp>
#include <exe/executors/hint.hpp>

namespace exe::executors {

struct IExecutor {
  virtual ~IExecutor() = default;

  // TODO: Support scheduler hints
  virtual void Submit(TaskBase* task) = 0;
};

}  // namespace exe::executors
