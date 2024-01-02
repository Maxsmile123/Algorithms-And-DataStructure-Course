#pragma once

#include <exe/futures/types/future.hpp>

namespace exe::futures::traits {

namespace match {

template <typename T>
struct ValueOf {};

template <typename T>
struct ValueOf<Future<T>> {
  using Type = T;
};

}  // namespace match

template <typename Future>
using ValueOf = typename match::ValueOf<Future>::Type;

}  // namespace exe::futures::traits
