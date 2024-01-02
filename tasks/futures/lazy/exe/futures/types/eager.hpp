#pragma once

#include <exe/futures/types/naked.hpp>

#include <exe/futures/thunks/eager.hpp>

namespace exe::futures {

template <typename T>
using EagerFuture = thunks::Eager<T>;

}  // namespace exe::futures
