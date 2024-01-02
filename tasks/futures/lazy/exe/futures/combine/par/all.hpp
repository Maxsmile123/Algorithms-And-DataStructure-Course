#pragma once

#include <exe/futures/types/future.hpp>

#include <exe/futures/traits/value_of.hpp>

#include <exe/futures/thunks/not_implemented.hpp>

#include <tuple>

namespace exe::futures {

template <SomeFuture LeftFuture, SomeFuture RightFuture>
Future<
    std::tuple<traits::ValueOf<LeftFuture>, traits::ValueOf<RightFuture>>> auto
Both(LeftFuture, RightFuture) {
  return thunks::NotImplemented<
      std::tuple<traits::ValueOf<LeftFuture>, traits::ValueOf<RightFuture>>>{};
}

// + variadic All

}  // namespace exe::futures
