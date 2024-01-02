#include <exe/executors/tp/fast/thread_pool.hpp>

namespace exe::executors::tp::fast {

ThreadPool::ThreadPool(size_t /*threads*/) {
  // Not implemented
}

ThreadPool::~ThreadPool() {
  // Not implemented
}

void ThreadPool::Execute(TaskBase* /*task*/) {
  // Not implemented
  // Use Worker::Current()
}

void ThreadPool::WaitIdle() {
  // Not implemented
}

void ThreadPool::Stop() {
  // Not implemented
}

ThreadPool* ThreadPool::Current() {
  return nullptr;  // Not implemented
}

}  // namespace exe::executors::tp::fast
