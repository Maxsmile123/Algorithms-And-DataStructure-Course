#pragma once

#include <exe/coroutine/routine.hpp>
#include <exe/executors/executor.hpp>

namespace exe::fibers {

using Routine = coroutine::Routine;

using Scheduler = executors::IExecutor;

// Considered harmful

// Starts a new fiber
void Go(Scheduler& scheduler, Routine routine);

// Starts a new fiber in the current scheduler
void Go(Routine routine);

namespace self {

void Yield();

// For synchronization primitives
// Do not use directly
void Suspend();

}  // namespace self

}  // namespace exe::fibers
