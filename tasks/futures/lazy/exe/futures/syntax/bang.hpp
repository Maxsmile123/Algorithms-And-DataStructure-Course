#pragma once

#include <exe/futures/types/future.hpp>

#include <exe/futures/combine/seq/start.hpp>

/*
 * "Bang" operator (!)
 *
 * Named after bang patterns in Strict Haskell
 * https://www.fpcomplete.com/haskell/tutorial/all-about-strictness/
 *
 */

template <exe::futures::SomeFuture InputFuture>
auto operator!(InputFuture f) {
  return std::move(f) | exe::futures::Start();
}
