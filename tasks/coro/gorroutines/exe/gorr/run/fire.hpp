#pragma once

#include <exe/gorr/core/task.hpp>

namespace exe::gorr {

inline void FireAndForget(Task<>&& task) {
  task.ReleaseCoroutine().resume();
}

}  // namespace exe::gorr
