#pragma once

#include <exe/futures/types/future.hpp>

namespace exe::futures::traits {

namespace match {

template <typename T>
struct ValueOf {};

template <SomeFuture F>
struct ValueOf<F> {
  using Type = typename F::ValueType;
};

}  // namespace match

template <typename Future>
using ValueOf = typename match::ValueOf<Future>::Type;

}  // namespace exe::futures::traits
