#pragma once

#include <exe/futures/syntax/pipe.hpp>

#include <exe/futures/traits/value_of.hpp>

#include <exe/futures/types/eager.hpp>

namespace exe::futures {

namespace pipe {

struct [[nodiscard]] Start {
  template <SomeFuture InputFuture>
  EagerFuture<traits::ValueOf<InputFuture>> Pipe(InputFuture) {
    std::abort();  // Not implemented
  }
};

}  // namespace pipe

// Future<T> -> EagerFuture<T>

inline auto Start() {
  return pipe::Start{};
}

inline auto Force() {
  return pipe::Start{};
}

}  // namespace exe::futures
