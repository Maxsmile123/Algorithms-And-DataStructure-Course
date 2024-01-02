#include <wheels/test/framework.hpp>

#include "../lock_guard.hpp"

#include <mutex>

TEST_SUITE(LockGuard) {
  SIMPLE_TEST(JustWorks) {
    std::mutex mutex;

    {
      LockGuard guard{mutex};

      ASSERT_FALSE(mutex.try_lock());
    }

    {
      ASSERT_TRUE(mutex.try_lock());
      mutex.unlock();
    }
  }

  SIMPLE_TEST(Unwind) {
    std::mutex mutex;

    try {
      {
        LockGuard guard{mutex};

        ASSERT_FALSE(mutex.try_lock());

        throw 1;
      }
    } catch (int) {
      // Ignore
    }

    {
      ASSERT_TRUE(mutex.try_lock());
      mutex.unlock();
    }
  }

  SIMPLE_TEST(BasicLockable) {
    struct NopMutex {
      void lock() {
        // Nop
      }

      void unlock() {
        // Nop
      }
    };

    NopMutex mutex;

    {
      LockGuard guard{mutex};
    }
  }
}

RUN_ALL_TESTS()
