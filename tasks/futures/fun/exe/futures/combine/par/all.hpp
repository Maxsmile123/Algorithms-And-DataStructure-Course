#pragma once

#include <exe/futures/types/future.hpp>

#include <tuple>

namespace exe::futures {

template <typename X, typename Y>
Future<std::tuple<X, Y>> Both(Future<X>, Future<Y>) {
  std::abort();  // Not implemented
}

// + variadic All

}  // namespace exe::futures
