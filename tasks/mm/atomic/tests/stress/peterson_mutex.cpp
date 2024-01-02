#include "../../atomic.hpp"

#include <wheels/test/framework.hpp>
#include <twist/test/budget.hpp>

// SpinLockPause
#include <wheels/support/cpu.hpp>

#include <twist/test/plate.hpp>

#include <thread>

//////////////////////////////////////////////////////////////////////

// Peterson (or Dekker) mutex for two threads

class PetersonMutex {
 public:
  PetersonMutex() {
    victim_ = 0;
    wants_[0] = 0;
    wants_[1] = 0;
  }

  // `me` \in {0, 1}
  void Lock(int me) {
    wants_[me] = 1;
    victim_ = me;
    while (wants_[1 - me] == 1 && victim_ == me) {
      Backoff();
    }
    owner_ = me;
  }

  void Unlock() {
    wants_[owner_] = 0;
  }

 private:
  void Backoff() {
    wheels::SpinLockPause();
  }

 private:
  stdlike::Atomic victim_{0};
  stdlike::Atomic wants_[2];
  size_t owner_;
};

//////////////////////////////////////////////////////////////////////

void MutualExlusionTest() {
  PetersonMutex mutex;
  twist::test::Plate plate;  // guarded by `mutex`

  auto make_contender = [&](size_t thread_index) {
    return [&, thread_index]() {
      while (twist::test::KeepRunning()) {
        mutex.Lock(thread_index);

        // Cs
        plate.Access();

        mutex.Unlock();
      }
    };
  };

  std::thread t0(make_contender(0));
  std::thread t1(make_contender(1));

  t0.join();
  t1.join();

  ASSERT_TRUE(plate.AccessCount() > 100500);
};

//////////////////////////////////////////////////////////////////////

TEST_SUITE(PetersonMutex) {
  SIMPLE_TEST(MutualExlusion) {
    MutualExlusionTest();
  }
}
