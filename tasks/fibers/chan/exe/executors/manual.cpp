#include <exe/executors/manual.hpp>

namespace exe::executors {

void ManualExecutor::Execute(TaskBase* /*task*/) {
  // Not implemented
}

// Run tasks

size_t ManualExecutor::RunAtMost(size_t /*limit*/) {
  return 0;  // Not implemented
}

size_t ManualExecutor::Drain() {
  return 0;  // Not implemented
}

}  // namespace exe::executors
