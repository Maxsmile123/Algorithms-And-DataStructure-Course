#pragma once

#include <exe/futures/types/naked.hpp>

#include <exe/futures/thunks/box.hpp>

namespace exe::futures {

template <typename T>
using BoxedFuture = thunks::Boxed<T>;

}  // namespace exe::futures
