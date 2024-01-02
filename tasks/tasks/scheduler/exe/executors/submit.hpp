#pragma once

#include <exe/executors/executor.hpp>

namespace exe::executors {

/*
 * Usage:
 *
 * Submit(thread_pool, [] {
 *   fmt::println("Running on thread pool");
 * });
 *
 */

template <typename F>
void Submit(IExecutor& exe, F&& fun) {
  std::abort();  // Not implemented
}

}  // namespace exe::executors
