#pragma once

#include <exe/executors/executor.hpp>

#include <cstdlib>

namespace exe::executors {

/*
 * Usage:
 * Execute(thread_pool, []() {
 *   std::cout << "Hi" << std::endl;
 * });
 */

template <typename F>
void Execute(IExecutor& /*where*/, F&& /*f*/) {
  std::abort();  // Not implemented
}

}  // namespace exe::executors