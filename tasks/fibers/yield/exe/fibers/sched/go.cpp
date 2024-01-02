#include <exe/fibers/sched/go.hpp>

#include <iostream>

namespace exe::fibers {

void Go(Scheduler& scheduler, Routine routine) {
  Fiber* fib = new Fiber(&scheduler, std::move(routine));
  fib->Schedule();
}

void Go(Routine routine) {
  Fiber* fib = new Fiber(exe::tp::ThreadPool::Current(), std::move(routine));
  fib->Schedule();
}

}  // namespace exe::fibers
