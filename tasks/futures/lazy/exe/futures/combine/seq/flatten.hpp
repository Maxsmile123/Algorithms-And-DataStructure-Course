#pragma once

#include <exe/futures/syntax/pipe.hpp>

#include <exe/futures/traits/value_of.hpp>

#include <exe/futures/thunks/not_implemented.hpp>

namespace exe::futures {

namespace pipe {

struct [[nodiscard]] Flatten {
  template <SomeFuture InputFuture>
  Future<traits::ValueOf<traits::ValueOf<InputFuture>>> auto Pipe(InputFuture) {
    return thunks::NotImplemented<
        traits::ValueOf<traits::ValueOf<InputFuture>>>{};
  }
};

}  // namespace pipe

// Future<Future<T>> -> Future<T>

inline auto Flatten() {
  return pipe::Flatten{};
}

}  // namespace exe::futures
