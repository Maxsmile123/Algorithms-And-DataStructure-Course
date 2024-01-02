#pragma once

namespace exe::gorr {

// Your awaiter goes here

// Precondition: coroutine is running in `current` executor
inline auto Yield() {
  return std::suspend_never{};  // Not implemented
}

}  // namespace exe::gorr
