#include <exe/coro/core.hpp>

#include <twist/ed/local/ptr.hpp>

#include <wheels/core/assert.hpp>
#include <wheels/core/compiler.hpp>

namespace exe::coro {

Coroutine::Coroutine(Routine /*routine*/) {
  // Not implemented
}

void Coroutine::Resume() {
  // Not implemented
}

void Coroutine::Suspend() {
  // Not implemented
}

bool Coroutine::IsCompleted() const {
  return true;  // Not implemented
}

}  // namespace exe::coro
