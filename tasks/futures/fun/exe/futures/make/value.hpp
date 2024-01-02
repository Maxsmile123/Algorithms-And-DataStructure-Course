#pragma once

#include <exe/futures/types/future.hpp>

namespace exe::futures {

template <typename T>
Future<T> Value(T) {
  std::abort();  // Not implemented
}

}  // namespace exe::futures
