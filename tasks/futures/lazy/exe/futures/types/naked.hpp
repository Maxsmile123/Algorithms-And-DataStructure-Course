#pragma once

#include <exe/futures/model/thunk.hpp>

namespace exe::futures {

template <typename F>
concept SomeFuture = Thunk<F>;

template <typename F, typename T>
concept Future = SomeFuture<F> && std::same_as<typename F::ValueType, T>;

}  // namespace exe::futures
