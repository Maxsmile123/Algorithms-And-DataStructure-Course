#include <exe/fibers/core/handle.hpp>

#include <exe/fibers/core/fiber.hpp>

#include <wheels/core/assert.hpp>

#include <utility>

namespace exe::fibers {

Fiber* FiberHandle::Release() {
  WHEELS_ASSERT(fiber_ != nullptr, "Invalid fiber handle");
  return std::exchange(fiber_, nullptr);
}

void FiberHandle::Schedule() {
  Release()->Schedule();
}

void FiberHandle::Resume() {
  // Not implemented
}

}  // namespace exe::fibers
