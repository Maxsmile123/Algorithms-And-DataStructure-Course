#pragma once

#include <exe/futures/syntax/pipe.hpp>

#include <exe/futures/traits/value_of.hpp>

#include <exe/futures/thunks/not_implemented.hpp>

#include <type_traits>

namespace exe::futures {

namespace pipe {

template <typename F>
struct [[nodiscard]] Map {
  F fun;

  explicit Map(F f)
      : fun(std::move(f)) {
  }

  template <typename T>
  using U = std::invoke_result_t<F, T>;

  template <SomeFuture InputFuture>
  Future<U<traits::ValueOf<InputFuture>>> auto Pipe(InputFuture) {
    return thunks::NotImplemented<U<traits::ValueOf<InputFuture>>>{};
  }
};

}  // namespace pipe

// Future<T> -> (T -> U) -> Future<U>

template <typename F>
auto Map(F fun) {
  return pipe::Map{std::move(fun)};
}

}  // namespace exe::futures
