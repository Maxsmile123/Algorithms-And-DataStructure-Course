#pragma once

#include <exe/futures/model/thunk.hpp>

namespace exe::futures::thunks {

template <typename T>
struct [[nodiscard]] Eager {
  using ValueType = T;

  Eager() {
    // Not implemented
  }

  // Non-copyable
  Eager(const Eager&) = delete;

  // Movable
  Eager(Eager&&) = default;

  void Start(IConsumer<T>*) {
    std::abort();  // Not implemented
  }
};

}  // namespace exe::futures::thunks
