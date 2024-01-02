#pragma once

#include <stdlike/future.hpp>

#include <memory>


namespace stdlike {
template <typename T>
class Promise {
 public:
  Promise(){
    shared_state_ = std::make_shared<stdlike::detail::Shared<T>>();
  };

  // Non-copyable
  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;

  // Movable
  Promise(Promise&&) = default;
  Promise& operator=(Promise&&) = default;

  // One-shot
  Future<T> MakeFuture() {
    return Future<T>(shared_state_);
  }

  // One-shot
  // Fulfill promise with value
  void SetValue(T value) {
    shared_state_->SetValue(std::move(value));
  }

  // One-shot
  // Fulfill promise with exception
  void SetException(std::exception_ptr eptr) {
    shared_state_->SetExc(eptr);
  }
  
 private:
  std::shared_ptr<stdlike::detail::Shared<T>> shared_state_;
};

}  // namespace stdlike
