#pragma once

#include <exe/gorr/core/task.hpp>

namespace exe::gorr {

// Blocks current thread
template <typename T>
T Run(Task<T>&& /*task*/) {
  std::abort();  // Not implemented
}

}  // namespace exe::gorr
