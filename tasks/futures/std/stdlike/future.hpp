#pragma once

#include <stdlike/shared.hpp>

#include <cassert>

namespace stdlike {

template <typename T>
class Future {
  template <typename U>
  friend class Promise;

 public:
  // Non-copyable
  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;

  // Movable
  Future(Future&&) = default;
  Future& operator=(Future&&) = default;

  // One-shot
  // Wait for result (value or exception)
  T Get() {
    return shared_state_->GetValue();
  }

 private:
  Future(
    std::shared_ptr<stdlike::detail::Shared<T>> shared_state
    ): shared_state_(shared_state){}

 private:
  std::shared_ptr<stdlike::detail::Shared<T>> shared_state_;
};

}  // namespace stdlike
