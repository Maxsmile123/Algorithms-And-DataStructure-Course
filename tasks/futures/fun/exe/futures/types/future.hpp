#pragma once

#include <cstdlib>
#include <utility>

namespace exe::futures {

template <typename T>
struct [[nodiscard]] Future {
  using ValueType = T;

  // Non-copyable
  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;

  // Non-copy-assignable
  Future& operator=(Future&) = delete;

  // Movable
  Future(Future&&) {
    // Not implemented
  }
};

}  // namespace exe::futures
