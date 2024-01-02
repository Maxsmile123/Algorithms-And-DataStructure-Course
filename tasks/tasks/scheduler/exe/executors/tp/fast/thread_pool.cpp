#include <exe/executors/tp/fast/thread_pool.hpp>

namespace exe::executors::tp::fast {

ThreadPool::ThreadPool(size_t threads)
    : threads_(threads) {
  // Not implemented
}

void ThreadPool::Start() {
  // Not implemented
}

ThreadPool::~ThreadPool() {
  // Not implemented
}

void ThreadPool::Submit(TaskBase* /*task*/) {
  // Not implemented
}

void ThreadPool::Stop() {
  // Not implemented
}

PoolMetrics ThreadPool::Metrics() const {
  std::abort();  // Not implemented
}

ThreadPool* ThreadPool::Current() {
  // Use Worker::Current()
  return nullptr;  // Not implemented
}

}  // namespace exe::executors::tp::fast
