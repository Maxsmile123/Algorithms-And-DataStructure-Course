#pragma once

#include <exe/futures/model/thunk.hpp>

#include <cstdlib>

namespace exe::futures::thunks {

template <typename T>
struct [[nodiscard]] Boxed {
  using ValueType = T;

  // Auto-boxing
  template <Thunk Thunk>
  Boxed(Thunk) {  // NOLINT
    // Not implemented
  }

  // Non-copyable
  Boxed(const Boxed&) = delete;

  // Movable
  Boxed(Boxed&&) = default;

  void Start(IConsumer<T>*) {
    std::abort();  // Not implemented
  }
};

}  // namespace exe::futures::thunks
