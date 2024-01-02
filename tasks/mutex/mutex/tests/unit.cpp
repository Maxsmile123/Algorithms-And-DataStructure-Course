#include "../mutex.hpp"

#include <wheels/test/framework.hpp>
#include <wheels/test/util/cpu_timer.hpp>

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

using stdlike::Mutex;

TEST_SUITE(UnitTest) {
  SIMPLE_TEST(LockUnlock) {
    Mutex mutex;
    mutex.Lock();
    mutex.Unlock();
  }

  SIMPLE_TEST(SequentialLockUnlock) {
    Mutex mutex;
    mutex.Lock();
    mutex.Unlock();
    mutex.Lock();
    mutex.Unlock();
  }

  SIMPLE_TEST(NoSharedLocations) {
    Mutex mutex;
    mutex.Lock();

    Mutex mutex2;
    mutex2.Lock();
    mutex2.Unlock();

    mutex.Unlock();
  }

  SIMPLE_TEST(MutualExclusion) {
    Mutex mutex;
    bool cs = false;

    std::thread locker([&]() {
      mutex.Lock();
      cs = true;
      std::this_thread::sleep_for(3s);
      cs = false;
      mutex.Unlock();
    });

    std::this_thread::sleep_for(1s);
    mutex.Lock();
    ASSERT_FALSE(cs);
    mutex.Unlock();

    locker.join();
  }

  SIMPLE_TEST(Blocking) {
    Mutex mutex;

    // Warmup
    mutex.Lock();
    mutex.Unlock();

    std::thread sleeper([&]() {
      mutex.Lock();
      std::this_thread::sleep_for(3s);
      mutex.Unlock();
    });

    std::thread waiter([&]() {
      std::this_thread::sleep_for(1s);

      wheels::ThreadCPUTimer cpu_timer;

      mutex.Lock();
      mutex.Unlock();

      auto spent = cpu_timer.Spent();

      ASSERT_TRUE(spent < 200ms);
    });

    sleeper.join();
    waiter.join();
  }
}

RUN_ALL_TESTS()
