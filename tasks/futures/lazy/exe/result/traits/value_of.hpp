#pragma once

#include <exe/result/types/result.hpp>

namespace exe::result::traits {

namespace match {

template <typename X>
struct ValueOf {};

template <typename T>
struct ValueOf<Result<T>> {
  using Type = T;
};

}  // namespace match

template <typename Result>
using ValueOf = typename match::ValueOf<Result>::Type;

}  // namespace exe::result::traits
