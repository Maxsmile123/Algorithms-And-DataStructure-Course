#pragma once

#include <thread_pool/static_thread_pool.hpp>

#include <experimental/coroutine>

namespace detail {

struct ThreadPoolAwaiter {
  bool await_ready() {
    return false;
  }

  void await_suspend(std::experimental::coroutine_handle<> handle) {
    pool.Submit([handle]() mutable {
      handle.resume();
    });
  }

  void await_resume() {
    // Nop
  }

  tp::StaticThreadPool& pool;
};

}  // namespace detail

inline auto ScheduleTo(tp::StaticThreadPool& pool) {
  return detail::ThreadPoolAwaiter{pool};
}
