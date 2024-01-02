#pragma once

#include <exe/futures/combine/par/all.hpp>

template <typename X, typename Y>
auto operator+(exe::futures::Future<X> f, exe::futures::Future<Y> g) {
  return exe::futures::Both(std::move(f), std::move(g));
}
