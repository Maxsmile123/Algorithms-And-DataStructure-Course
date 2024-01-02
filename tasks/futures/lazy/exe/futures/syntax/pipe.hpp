#pragma once

#include <exe/futures/types/naked.hpp>

template <exe::futures::SomeFuture Future, typename C>
auto operator|(Future f, C c) {
  return c.Pipe(std::move(f));
}
