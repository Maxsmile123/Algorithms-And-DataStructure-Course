#pragma once

#include <exe/result/types/result.hpp>

#include <exe/futures/model/context.hpp>

namespace exe::futures {

template <typename T>
struct Output {
  Result<T> result;
  Context context;
};

}  // namespace exe::futures
