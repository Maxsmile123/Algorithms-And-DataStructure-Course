#include <exe/executors/tp/compute/thread_pool.hpp>

#include <twist/ed/local/ptr.hpp>

namespace exe::executors::tp::compute {

////////////////////////////////////////////////////////////////////////////////

static twist::ed::ThreadLocalPtr<ThreadPool> pool;

////////////////////////////////////////////////////////////////////////////////

ThreadPool::ThreadPool(size_t /*threads*/) {
  // Not implemented
}

ThreadPool::~ThreadPool() {
  // Not implemented
}

void ThreadPool::Execute(TaskBase* /*task*/) {
  // Not implemented
}

void ThreadPool::WaitIdle() {
  // Not implemented
}

void ThreadPool::Stop() {
  // Not implemented
}

ThreadPool* ThreadPool::Current() {
  return pool;
}

}  // namespace exe::executors::tp::compute
