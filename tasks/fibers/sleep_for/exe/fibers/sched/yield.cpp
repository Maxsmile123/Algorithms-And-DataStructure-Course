#include <exe/fibers/sched/yield.hpp>

namespace exe::fibers {

void Yield() {
  exe::coro::Coroutine::Suspend();
}

}  // namespace exe::fibers
