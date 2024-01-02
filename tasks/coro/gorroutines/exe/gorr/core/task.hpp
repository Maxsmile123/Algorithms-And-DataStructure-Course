#pragma once

#include <cstdlib>
#include <optional>

#include <exe/result/types/unit.hpp>
#include <exe/result/types/result.hpp>

#include <coroutine>

namespace exe::gorr {

//////////////////////////////////////////////////////////////////////

template <typename T = Unit>
struct Task {
  struct Promise {
    // NOLINTNEXTLINE
    auto get_return_object() {
      return Task{
          std::coroutine_handle<Promise>::from_promise(*this)};
    }

    // NOLINTNEXTLINE
    auto initial_suspend() noexcept {
      return std::suspend_always{};
    }

    // NOLINTNEXTLINE
    auto final_suspend() noexcept {
      return std::suspend_never{};
    }

    // NOLINTNEXTLINE
    void set_exception(std::exception_ptr) {
      std::terminate();  // Not implemented
    }

    // NOLINTNEXTLINE
    void unhandled_exception() {
      std::terminate();  // Not implemented
    }

    // NOLINTNEXTLINE
    void return_void() {
      // Not implemented
    }

    std::optional<Result<T>> result;
  };

  using CoroutineHandle = std::coroutine_handle<Promise>;

  explicit Task(CoroutineHandle callee) : callee_(callee) {
  }

  Task(Task&&) = default;

  // Non-copyable
  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;

  ~Task() {
    if (callee_ && !callee_.done()) {
      std::terminate();
    }
  }

  CoroutineHandle ReleaseCoroutine() {
    return std::exchange(callee_, CoroutineHandle());
  }

 private:
  CoroutineHandle callee_;
};

}  // namespace exe::gorr

//////////////////////////////////////////////////////////////////////

template <typename T, typename... Args>
struct std::coroutine_traits<exe::gorr::Task<T>, Args...> {
  using promise_type = typename exe::gorr::Task<T>::Promise;  // NOLINT
};
