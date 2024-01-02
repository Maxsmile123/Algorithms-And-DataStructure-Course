#pragma once

#include <exe/futures/types/future.hpp>

#include <exe/futures/thunks/not_implemented.hpp>

#include <exe/result/types/error.hpp>

namespace exe::futures {

template <typename T>
Future<T> auto Failure(Error /*with*/) {
  return thunks::NotImplemented<T>{};
}

}  // namespace exe::futures
