#pragma once

#include <exe/fibers/core/api.hpp>
#include <exe/coroutine/impl.hpp>
#include <sure/stack.hpp>

namespace exe::fibers {

// Fiber = Stackful coroutine + Executor

class Fiber {
 public:
  // ~ System calls

  void Schedule();

  void Yield();

  void Suspend();
  void Resume();

  static Fiber& Self();

 private:
  // Task
  void Step();

 private:
  // ???
};

}  // namespace exe::fibers
