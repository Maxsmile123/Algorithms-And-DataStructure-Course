#include <exe/coroutine/impl.hpp>

#include <wheels/core/assert.hpp>
#include <wheels/core/compiler.hpp>

namespace exe::coroutine {

CoroutineImpl::CoroutineImpl(Routine /*routine*/,
                             wheels::MutableMemView /*stack*/) {
}

void CoroutineImpl::Run() {
  std::abort();  // Not implemented
}

void CoroutineImpl::Resume() {
  // Not implemented
}

void CoroutineImpl::Suspend() {
  // Not implemented
}

bool CoroutineImpl::IsCompleted() const {
  return true;  // Not implemented
}

}  // namespace exe::coroutine
