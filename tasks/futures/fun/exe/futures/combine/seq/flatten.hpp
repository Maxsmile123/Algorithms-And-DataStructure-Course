#pragma once

#include <exe/futures/syntax/pipe.hpp>

namespace exe::futures {

namespace pipe {

struct [[nodiscard]] Flatten {
  template <typename T>
  Future<T> Pipe(Future<Future<T>>) {
    std::abort();  // Not implemented
  }
};

}  // namespace pipe

// Future<Future<T>> -> Future<T>

inline auto Flatten() {
  return pipe::Flatten{};
}

}  // namespace exe::futures
