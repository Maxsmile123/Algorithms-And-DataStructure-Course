#pragma once

#include <exe/executors/executor.hpp>

namespace exe::executors {

// Executes task immediately on the current thread

IExecutor& Inline();

}  // namespace exe::executors
