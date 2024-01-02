#include <wheels/test/framework.hpp>

#include "../barrier.hpp"

// https://gitlab.com/Lipovsky/tinyfibers
#include <tf/sched/spawn.hpp>
#include <tf/sched/yield.hpp>
#include <tf/sync/mutex.hpp>
#include <tf/sync/wait_group.hpp>

using tf::Mutex;
using tf::Spawn;
using tf::WaitGroup;
using tf::Yield;

void TwoFibersDeadLock() {
  Mutex a;
  Mutex b;

  auto first = [&] {
    a.Lock();
    Yield();
    b.Lock();
    b.Unlock();
    a.Unlock();
  };

  auto second = [&] {
    b.Lock();
    Yield();
    a.Lock();
    a.Unlock();
    b.Unlock();
  };

  Spawn(first).Join();
  Spawn(first).Join();

  Spawn(second).Join();
  Spawn(second).Join();

  ReadyToDeadLock();

  WaitGroup wg;
  wg.Spawn(first).Spawn(second).Wait();

  FAIL_TEST("No deadlock =(");
}
