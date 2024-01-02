#include "../spinlock.hpp"
#include "../atomic_ops.hpp"

#include <wheels/test/framework.hpp>

TEST_SUITE(Atomics) {
  SIMPLE_TEST(LoadStore) {
    AtomicInt64 test = 0;

    ASSERT_EQ(AtomicLoad(&test), 0);
    AtomicStore(&test, 42);
    ASSERT_EQ(AtomicLoad(&test), 42);
  }

  SIMPLE_TEST(Exchange) {
    AtomicInt64 test = 0;

    ASSERT_EQ(AtomicExchange(&test, 7), 0);
    ASSERT_EQ(AtomicLoad(&test), 7);
    ASSERT_EQ(AtomicExchange(&test, 11), 7);
    ASSERT_EQ(AtomicExchange(&test, 42), 11);
  }
}

TEST_SUITE(SpinLock) {
  SIMPLE_TEST(LockUnlock) {
    TASSpinLock spinlock;

    spinlock.Lock();
    spinlock.Unlock();
  }

  SIMPLE_TEST(SequentialLockUnlock) {
    TASSpinLock spinlock;

    spinlock.Lock();
    spinlock.Unlock();

    spinlock.Lock();
    spinlock.Unlock();
  }

  SIMPLE_TEST(TryLock) {
    TASSpinLock spinlock;

    ASSERT_TRUE(spinlock.TryLock());
    ASSERT_FALSE(spinlock.TryLock());
    spinlock.Unlock();
    ASSERT_TRUE(spinlock.TryLock());
    spinlock.Unlock();
    spinlock.Lock();
    ASSERT_FALSE(spinlock.TryLock());
  }
}

RUN_ALL_TESTS()
