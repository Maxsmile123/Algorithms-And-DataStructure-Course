#include "../coroutine.hpp"

#include <fmt/core.h>

#include <cassert>

int main() {
  Coroutine coro([] {
    Coroutine::Suspend();
  });

  coro.Resume();
  coro.Resume();

  assert(coro.IsCompleted());

  return 0;
}
