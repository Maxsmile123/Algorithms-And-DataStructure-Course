#include <wheels/test/framework.hpp>

// https://gitlab.com/Lipovsky/tinyfibers
#include <tf/sched/spawn.hpp>
#include <tf/sched/yield.hpp>
#include <tf/sync/mutex.hpp>

using tf::Mutex;
using tf::Spawn;
using tf::Yield;

void OneFiberDeadLock() {
  Mutex mutex;

  auto fiber = [&] {
    mutex.Lock();
    mutex.Lock();
  };

  Spawn(fiber).Join();

  FAIL_TEST("No deadlock =(");
}
