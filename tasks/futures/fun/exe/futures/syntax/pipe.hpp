#pragma once

#include <exe/futures/types/future.hpp>

template <typename T, typename C>
auto operator|(exe::futures::Future<T> f, C c) {
  return c.Pipe(std::move(f));
}
