#pragma once

#include <exe/executors/executor.hpp>

#include <coroutine>

namespace exe::gorr {

namespace detail {

// Your awaiter goes here

}  // namespace detail

// Reschedule current coroutine to executor `target`
inline auto TeleportTo(executors::IExecutor& /*target*/) {
  return std::suspend_never{};  // Not implemented
}

}  // namespace exe::gorr
