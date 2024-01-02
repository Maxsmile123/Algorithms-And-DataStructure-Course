#pragma once

#include <exe/futures/syntax/pipe.hpp>

#include <exe/futures/traits/value_of.hpp>
#include <exe/result/traits/value_of.hpp>

#include <exe/futures/thunks/not_implemented.hpp>

#include <type_traits>

namespace exe::futures {

namespace pipe {

template <typename F>
struct [[nodiscard]] AndThen {
  F fun;

  explicit AndThen(F f)
      : fun(std::move(f)) {
  }

  // Non-copyable
  AndThen(AndThen&) = delete;

  template <typename T>
  using U = result::traits::ValueOf<std::invoke_result_t<F, T>>;

  template <SomeFuture InputFuture>
  Future<U<traits::ValueOf<InputFuture>>> auto Pipe(InputFuture) {
    return thunks::NotImplemented<U<traits::ValueOf<InputFuture>>>{};
  }
};

}  // namespace pipe

// Future<T> -> (T -> Result<U>) -> Future<U>

template <typename F>
auto AndThen(F fun) {
  return pipe::AndThen{std::move(fun)};
}

}  // namespace exe::futures
