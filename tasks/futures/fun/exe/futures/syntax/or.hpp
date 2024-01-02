#pragma once

#include <exe/futures/combine/par/first.hpp>

template <typename T>
exe::futures::Future<T> operator||(exe::futures::Future<T> f,
                                   exe::futures::Future<T> g) {
  return exe::futures::First(std::move(f), std::move(g));
}
