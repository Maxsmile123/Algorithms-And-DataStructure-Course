#include "../../atomic.hpp"

#include <wheels/test/framework.hpp>

#include <thread>

//////////////////////////////////////////////////////////////////////

using stdlike::Atomic;
using stdlike::MemoryOrder;

//////////////////////////////////////////////////////////////////////

bool MaybeStoreBuffering() {
  // Shared
  Atomic x{0};
  Atomic y{0};

  // Local
  int r1;
  int r2;

  std::thread t1([&]() {
    x.Store(1, MemoryOrder::Release);
    r1 = y.Load(MemoryOrder::Acquire);
  });

  std::thread t2([&]() {
    y.Store(1, MemoryOrder::Release);
    r2 = x.Load(MemoryOrder::Acquire);
  });

  t1.join();
  t2.join();

  return (r1 == 0) && (r2 == 0);
}

void StoreBufferingHappens() {
  while (!MaybeStoreBuffering()) {
    ;  // Keep trying
  }
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(StoreBuffering) {
  SIMPLE_TEST(Happens) {
    StoreBufferingHappens();
  }
}
