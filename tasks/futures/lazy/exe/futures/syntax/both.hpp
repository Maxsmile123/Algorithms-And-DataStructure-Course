#pragma once

#include <exe/futures/combine/par/all.hpp>

template <exe::futures::SomeFuture LeftFuture,
          exe::futures::SomeFuture RightFuture>
auto operator+(LeftFuture f, RightFuture g) {
  return exe::futures::Both(std::move(f), std::move(g));
}
