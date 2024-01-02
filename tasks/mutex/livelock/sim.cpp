#include <wheels/test/framework.hpp>

// https://gitlab.com/Lipovsky/tinyfibers
#include <tf/sched/spawn.hpp>
#include <tf/sched/yield.hpp>
#include <tf/sync/wait_group.hpp>

using tf::WaitGroup;
using tf::Yield;

void LiveLock() {
  static const size_t kIterations = 100;

  size_t cs_count = 0;

  // TrickyLock state
  size_t thread_count = 0;

  auto contender = [&] {
    for (size_t i = 0; i < kIterations; ++i) {
      // TrickyLock::Lock
      while (thread_count++ > 0) {
        Yield();
        --thread_count;
      }
      // Spinlock acquired

      {
        // Critical section
        Yield();
        ++cs_count;
        ASSERT_TRUE_M(cs_count < 3, "Too many critical sections");
        // End of critical section
      }

      // TrickyLock::Unlock
      --thread_count;

      // Spinlock released
    }
  };

  // Spawn two fibers
  WaitGroup wg;
  wg.Spawn(contender).Spawn(contender).Wait();
};
