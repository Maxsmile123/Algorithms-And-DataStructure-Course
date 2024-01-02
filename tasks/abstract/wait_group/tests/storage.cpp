#include "../wait_group.hpp"

#include <course/twist/test.hpp>

#include <twist/ed/stdlike/thread.hpp>

//////////////////////////////////////////////////////////////////////

void StorageTest() {
  // Help AddressSanitizer
  auto* wg = new WaitGroup{};

  wg->Add(1);
  twist::ed::stdlike::thread t([wg] {
    wg->Done();
  });

  wg->Wait();
  delete wg;

  t.join();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(WaitGroup) {
  TWIST_TEST_REPEAT(Storage, 5s) {
    StorageTest();
  }
}

RUN_ALL_TESTS()
