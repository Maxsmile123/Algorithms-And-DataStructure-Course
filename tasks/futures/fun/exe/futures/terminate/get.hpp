#pragma once

#include <exe/futures/syntax/pipe.hpp>

#include <exe/result/types/result.hpp>

namespace exe::futures {

namespace pipe {

struct [[nodiscard]] Get {
  template <typename T>
  Result<T> Pipe(Future<T>) {
    std::abort();  // Not implemented
  }
};

}  // namespace pipe

inline auto Get() {
  return pipe::Get{};
}

}  // namespace exe::futures
