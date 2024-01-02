#pragma once

#include <sure/stack.hpp>

namespace exe::fibers {

sure::Stack AllocateStack();
void ReleaseStack(sure::Stack stack);

}  // namespace exe::fibers
