#include <exe/executors/tp/fast/worker.hpp>
#include <exe/executors/tp/fast/thread_pool.hpp>

#include <twist/ed/local/ptr.hpp>

namespace exe::executors::tp::fast {

Worker::Worker(ThreadPool& host, size_t index)
    : host_(host),
      index_(index) {
}

void Worker::Start() {
  thread_.emplace([this]() {
    Work();
  });
}

void Worker::Join() {
  thread_->join();
}

void Worker::Push(TaskBase* /*task*/, SchedulerHint /*hint*/) {
  // Not implemented
}

TaskBase* Worker::PickTask() {
  // Poll in order:
  // * [%61] Global queue
  // * LIFO slot
  // * Local queue
  // * Global queue
  // * Work stealing
  // Then
  //   Park worker

  return nullptr;  // Not implemented
}

void Worker::Work() {
  // Not implemented

  while (TaskBase* next = PickTask()) {
    next->Run();
  }
}

}  // namespace exe::executors::tp::fast
