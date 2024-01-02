#pragma once

#include <exe/futures/syntax/pipe.hpp>

#include <exe/futures/traits/value_of.hpp>

#include <exe/futures/thunks/not_implemented.hpp>

#include <type_traits>

namespace exe::futures {

namespace pipe {

template <typename F>
struct [[nodiscard]] FlatMap {
  F fun;

  explicit FlatMap(F f)
      : fun(std::move(f)) {
  }

  template <typename T>
  using U = traits::ValueOf<std::invoke_result_t<F, T>>;

  template <SomeFuture InputFuture>
  Future<U<traits::ValueOf<InputFuture>>> auto Pipe(InputFuture) {
    return thunks::NotImplemented<U<traits::ValueOf<InputFuture>>>{};
  }
};

}  // namespace pipe

// Future<T> -> (T -> Future<U>) -> Future<U>

template <typename F>
auto FlatMap(F fun) {
  return pipe::FlatMap{std::move(fun)};
}

}  // namespace exe::futures
