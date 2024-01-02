#include <exe/fibers/core/fiber.hpp>

#include <twist/ed/local/ptr.hpp>

#include <iostream>

namespace exe::fibers {

TWISTED_THREAD_LOCAL_PTR(Fiber, current);

Fiber::Fiber(Scheduler* sched, Routine routine): 
  coro_(std::move(routine)),
  scheduler_(sched)
{}

void Fiber::Schedule() {
  scheduler_->Submit([this](){
    this->Run();
    if (this->coro_.IsCompleted()) {
      delete this;
    } else {
      this->Schedule();
    }
  });
}

void Fiber::Run() {
  current = this;
  this->coro_.Resume();
}

Fiber* Fiber::Self() {
  return current;
}

}  // namespace exe::fibers
