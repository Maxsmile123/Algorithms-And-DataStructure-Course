#pragma once

#include <exe/futures/model/thunk.hpp>

#include <cstdlib>

namespace exe::futures::thunks {

template <typename T>
struct [[nodiscard]] NotImplemented {
  using ValueType = T;

  NotImplemented() = default;

  // Non-copyable
  NotImplemented(const NotImplemented&) = delete;

  // Movable
  NotImplemented(NotImplemented&&) = default;

  void Start(IConsumer<T>*) {
    std::abort();  // Not implemented
  }
};

}  // namespace exe::futures::thunks
