#pragma once

#include <exe/futures/syntax/pipe.hpp>

namespace exe::futures {

namespace pipe {

struct [[nodiscard]] Detach {
  template <typename T>
  void Pipe(Future<T>) {
    std::abort();  // Not implemented
  }
};

}  // namespace pipe

inline auto Detach() {
  return pipe::Detach{};
}

}  // namespace exe::futures
