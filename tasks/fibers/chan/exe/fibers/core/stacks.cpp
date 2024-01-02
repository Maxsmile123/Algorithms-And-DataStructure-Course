#include <exe/fibers/core/stacks.hpp>

using sure::Stack;

namespace exe::fibers {

//////////////////////////////////////////////////////////////////////

class StackAllocator {
 public:
  Stack Allocate() {
    return AllocateNewStack();  // Your code goes here
  }

  void Release(Stack /*stack*/) {
    // Your code goes here
  }

 private:
  static Stack AllocateNewStack() {
    static const size_t kStackPages = 16;  // 16 * 4KB = 64KB
    return Stack::AllocatePages(kStackPages);
  }

 private:
  // Pool
};

//////////////////////////////////////////////////////////////////////

StackAllocator allocator;

sure::Stack AllocateStack() {
  return allocator.Allocate();
}

void ReleaseStack(sure::Stack stack) {
  allocator.Release(std::move(stack));
}

}  // namespace exe::fibers
