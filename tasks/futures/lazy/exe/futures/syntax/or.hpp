#pragma once

#include <exe/futures/combine/par/first.hpp>

template <exe::futures::SomeFuture LeftFuture,
          exe::futures::SomeFuture RightFuture>
auto operator||(LeftFuture f, RightFuture g) {
  return exe::futures::First(std::move(f), std::move(g));
}
