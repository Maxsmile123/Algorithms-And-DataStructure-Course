#pragma once

#include <utility>

namespace exe::tp {

template <typename P, typename F>
void Submit(P& pool, F&& fun) {
  pool.Submit(std::forward<F>(fun));
}

}  // namespace exe::tp
