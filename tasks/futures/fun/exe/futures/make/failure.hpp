#pragma once

#include <exe/futures/types/future.hpp>

#include <exe/result/types/error.hpp>

namespace exe::futures {

template <typename T>
Future<T> Failure(Error /*with*/) {
  std::abort();  // Not implemented
}

}  // namespace exe::futures
