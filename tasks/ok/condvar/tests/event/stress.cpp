#include "../../mutex.hpp"
#include "../../condvar.hpp"

#include <course/twist/test.hpp>

#include <twist/ed/stdlike/thread.hpp>

// std::unique_lock / std::lock_guard
#include <mutex>

//////////////////////////////////////////////////////////////////////

class OneShotEvent {
 public:
  void Wait() {
    std::unique_lock locker(mutex_);
    while (!fired_) {
      fired_cond_.Wait(locker);
    }
  }

  void Fire() {
    std::lock_guard guard(mutex_);
    fired_ = true;
    fired_cond_.NotifyOne();
  }

 private:
  bool fired_{false};
  stdlike::Mutex mutex_;
  stdlike::CondVar fired_cond_;
};

//////////////////////////////////////////////////////////////////////

void StorageTest() {
  // Help AddressSanitizer
  auto event = new OneShotEvent{};

  twist::ed::stdlike::thread t([event] {
    event->Fire();
  });

  event->Wait();
  delete event;

  t.join();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(Event) {
  TWIST_TEST_REPEAT(Storage, 5s) {
    StorageTest();
  }
}

RUN_ALL_TESTS()
