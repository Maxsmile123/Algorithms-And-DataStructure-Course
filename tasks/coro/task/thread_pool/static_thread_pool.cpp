#include <thread_pool/static_thread_pool.hpp>

namespace tp {

StaticThreadPool::StaticThreadPool(size_t threads)
    : work_guard_(asio::make_work_guard(io_context_)) {
  StartWorkerThreads(/*count=*/threads);
}

StaticThreadPool::~StaticThreadPool() {
  Join();
}

void StaticThreadPool::Submit(Task task) {
  asio::post(io_context_, std::move(task));
}

static thread_local StaticThreadPool* pool{nullptr};

StaticThreadPool* StaticThreadPool::Current() {
  return pool;
}

void StaticThreadPool::Join() {
  if (joined_) {
    return;
  }
  work_guard_.reset();
  for (auto& worker : workers_) {
    worker.join();
  }
  joined_ = true;
}

void StaticThreadPool::Work() {
  pool = this;
  io_context_.run();  // Invoke posted handlers
}

void StaticThreadPool::StartWorkerThreads(size_t count) {
  for (size_t i = 0; i < count; ++i) {
    workers_.emplace_back([this]() {
      Work();
    });
  }
}

}  // namespace tp
