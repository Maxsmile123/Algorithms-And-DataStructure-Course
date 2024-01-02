#include "../atomic_stamped_ptr.hpp"
#include "../lock_free_stack.hpp"

#include <fmt/core.h>

int main() {
  LockFreeStack<int> stack;

  stack.Push(7);
  auto item = stack.TryPop();

  fmt::println("TryPop -> {}", *item);

  return 0;
}
