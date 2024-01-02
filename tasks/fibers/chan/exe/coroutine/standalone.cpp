#include <exe/coroutine/standalone.hpp>

#include <twist/ed/local/ptr.hpp>

#include <wheels/core/assert.hpp>
#include <wheels/core/defer.hpp>

namespace exe::coroutine {

static twist::ed::ThreadLocalPtr<Coroutine> current;

Coroutine::Coroutine(Routine routine)
    : stack_(AllocateStack()), impl_(std::move(routine), stack_.View()) {
}

void Coroutine::Resume() {
  Coroutine* prev = current.Exchange(this);

  wheels::Defer rollback([prev]() {
    current = prev;
  });

  impl_.Resume();
}

void Coroutine::Suspend() {
  WHEELS_VERIFY(current, "Not a coroutine");
  current->impl_.Suspend();
}

sure::Stack Coroutine::AllocateStack() {
  static const size_t kStackPages = 16;  // 16 * 4KB = 64KB
  return sure::Stack::AllocatePages(kStackPages);
}

}  // namespace exe::coroutine
