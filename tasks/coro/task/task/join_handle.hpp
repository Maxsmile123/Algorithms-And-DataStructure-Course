#pragma once

#include <wheels/support/result.hpp>

#include <experimental/coroutine>

#include "task.hpp"

#include <optional>

namespace task {

// Better:
// using JoinHandle = Task<void>;

class [[nodiscard]] JoinHandle {
 public:
  struct Promise;

  using AnyCoroHandle = std::experimental::coroutine_handle<>;
  using MyCoroHandle = std::experimental::coroutine_handle<Promise>;

  struct Promise {
    auto get_return_object() {  // NOLINT
      return JoinHandle{};
    }

    auto initial_suspend() noexcept {  // NOLINT
      // Not implemented
      return std::experimental::suspend_never{};
    }

    auto final_suspend() noexcept {  // NOLINT
      // Not implemented
      return std::experimental::suspend_never{};
    }

    void unhandled_exception() {  // NOLINT
      std::abort();  // Not implemented
    }

    void return_void() {  // NOLINT
      std::abort();  // Not implemented
    }
  };

  struct Awaiter {
    bool await_ready() {  // NOLINT
      return true;  // Not implemented
    }

    void await_suspend(AnyCoroHandle /*caller*/) {  // NOLINT
      // Not implemented
    }

    void await_resume() {  // NOLINT
      // Nop
    }
  };

  JoinHandle() {
    // Not implemented
  }

  JoinHandle(JoinHandle&&) {
    // Not implemented
  }

  JoinHandle& operator=(JoinHandle&&) {
    return *this;  // Not implemented
  }

  // Non-copyable
  JoinHandle(const JoinHandle&) = delete;
  JoinHandle& operator=(const JoinHandle&) = delete;

  ~JoinHandle() {
    // Not implemented
  }

  auto operator co_await() {
    return Awaiter{};
  }

 private:
  MyCoroHandle coro_;
};

}  // namespace task

template <typename... Args>
struct std::experimental::coroutine_traits<task::JoinHandle, Args...> {  // NOLINT
  using promise_type = task::JoinHandle::Promise;  // NOLINT
};
