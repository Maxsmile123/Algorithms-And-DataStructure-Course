#pragma once

#include <exe/futures/types/future.hpp>

namespace exe::futures {

template <typename T>
Future<T> First(Future<T>, Future<T>) {
  std::abort();  // Not implemented
}

}  // namespace exe::futures
