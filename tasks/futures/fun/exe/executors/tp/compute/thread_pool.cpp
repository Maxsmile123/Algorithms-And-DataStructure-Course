#include <exe/executors/tp/compute/thread_pool.hpp>

#include <twist/ed/local/ptr.hpp>

#include <wheels/core/panic.hpp>

namespace exe::executors::tp::compute {

ThreadPool::ThreadPool(size_t /*threads*/) {
  // Not implemented
}

void ThreadPool::Start() {
  // Not implemented
}

ThreadPool::~ThreadPool() {
  // Not implemented
}

void ThreadPool::Submit(Task /*task*/) {
  // Not implemented
}

ThreadPool* ThreadPool::Current() {
  return nullptr;  // Not implemented
}

void ThreadPool::WaitIdle() {
  // Not implemented
}

void ThreadPool::Stop() {
  // Not implemented
}

}  // namespace exe::executors::tp::compute
