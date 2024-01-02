#include "../../shared_ptr.hpp"

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/stdlike/thread.hpp>

#include <course/twist/test.hpp>

void StressTest() {
  AtomicSharedPtr<std::string> asp;

  asp.Store(MakeShared<std::string>("Initial"));

  twist::ed::stdlike::thread reader([&] {
    auto sp = asp.Load();
    ASSERT_TRUE(sp);
  });

  twist::ed::stdlike::thread writer([&] {
    asp.Store(MakeShared<std::string>("Rewritten"));
  });

  reader.join();
  writer.join();
}

TEST_SUITE(AtomicSharedPtr) {
  TWIST_TEST_REPEAT(Race, 5s) {
    StressTest();
  }
}

RUN_ALL_TESTS()
