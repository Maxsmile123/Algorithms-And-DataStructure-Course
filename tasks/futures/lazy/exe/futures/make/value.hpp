#pragma once

#include <exe/futures/types/future.hpp>

#include <exe/futures/thunks/not_implemented.hpp>

namespace exe::futures {

template <typename T>
Future<T> auto Value(T) {
  return thunks::NotImplemented<T>{};
}

}  // namespace exe::futures
