#pragma once

#include <exe/futures/types/future.hpp>

#include <exe/futures/traits/value_of.hpp>

#include <exe/futures/thunks/not_implemented.hpp>

#include <type_traits>

namespace exe::futures {

template <SomeFuture LeftFuture, SomeFuture RightFuture>
Future<traits::ValueOf<LeftFuture>> auto First(LeftFuture, RightFuture) {
  static_assert(std::is_same_v<traits::ValueOf<LeftFuture>,
                               traits::ValueOf<RightFuture>>);

  return thunks::NotImplemented<traits::ValueOf<LeftFuture>>{};
}

}  // namespace exe::futures
