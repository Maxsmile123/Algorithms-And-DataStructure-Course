#include <exe/fibers/sched/go.hpp>

namespace exe::fibers {

void Go(Scheduler& scheduler, Routine routine) {
  Fiber* fib = new Fiber(&scheduler, std::move(routine));
  fib->Schedule();
}

void Go(Routine routine) {
  Fiber* fib = new Fiber(Fiber::Self()->scheduler_, std::move(routine));
  fib->Schedule();
}

}  // namespace exe::fibers
