#pragma once

#include <exe/futures/syntax/pipe.hpp>

#include <exe/futures/traits/value_of.hpp>

#include <exe/futures/types/boxed.hpp>

namespace exe::futures {

namespace pipe {

struct [[nodiscard]] Box {
  template <SomeFuture InputFuture>
  BoxedFuture<traits::ValueOf<InputFuture>> Pipe(InputFuture) {
    std::abort();  // Not implemented
  }
};

}  // namespace pipe

// Future<T> -> BoxedFuture<T>

inline auto Box() {
  return pipe::Box{};
}

}  // namespace exe::futures
