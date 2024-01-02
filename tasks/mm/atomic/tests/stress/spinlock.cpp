#include "../../atomic.hpp"

#include <wheels/test/framework.hpp>
#include <twist/test/budget.hpp>

// SpinLockPause
#include <wheels/support/cpu.hpp>

#include <twist/test/plate.hpp>

#include <thread>
#include <vector>

//////////////////////////////////////////////////////////////////////

using stdlike::Atomic;
using stdlike::MemoryOrder;

//////////////////////////////////////////////////////////////////////

class SpinLock {
 public:
  void Lock() {
    while (locked_.Exchange(1, MemoryOrder::Acquire) == 1) {
      Backoff();
    }
  }

  void Unlock() {
    locked_.Store(0, MemoryOrder::Release);
  }

 private:
  void Backoff() {
    wheels::SpinLockPause();
  }

 private:
  Atomic locked_{0};
};

//////////////////////////////////////////////////////////////////////

void MutualExlusionTest(size_t threads) {
  SpinLock spinlock;
  twist::test::Plate plate;  // guarded by `spinlock`


  auto contender_routine = [&]() {
    while (twist::test::KeepRunning()) {
      spinlock.Lock();

      // Cs
      plate.Access();

      spinlock.Unlock();
    }
  };

  std::vector<std::thread> contenders;
  for (size_t i = 0; i < threads; ++i) {
    contenders.emplace_back(contender_routine);
  }

  for (auto& t : contenders) {
    t.join();
  }

  ASSERT_TRUE(plate.AccessCount() > 100500);
};

//////////////////////////////////////////////////////////////////////

TEST_SUITE(SpinLock) {
  SIMPLE_TEST(MutualExlusion) {
    MutualExlusionTest(4);
  }
}
