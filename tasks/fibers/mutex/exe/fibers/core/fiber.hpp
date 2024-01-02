#pragma once

#include <exe/fibers/core/routine.hpp>
#include <exe/fibers/core/scheduler.hpp>

#include <exe/coro/core.hpp>

namespace exe::fibers {

// Fiber = stackful coroutine + scheduler (executor)

class Fiber {
 public:
  void Suspend(/*???*/);

  void Schedule();
  void Switch();

  // Task
  void Run();

  static Fiber* Self();

 private:
  // ???
};

}  // namespace exe::fibers
