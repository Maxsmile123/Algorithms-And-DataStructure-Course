#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/sys.hpp>

#include <cstdint>

namespace exe::threads::stdlike {

class CondVar {
 public:
  // Mutex - BasicLockable
  // https://en.cppreference.com/w/cpp/named_req/BasicLockable
  template <class Mutex>
  void Wait(Mutex& mutex) {
    auto num = counter_.load();
    mutex.unlock();
    twist::ed::futex::Wait(counter_, num);
    mutex.lock();
  }

  template <class Mutex>
  void wait(Mutex& mutex) {
    Wait<Mutex>(mutex);
  }

  void NotifyOne() {
    auto wake_key = twist::ed::futex::PrepareWake(counter_);
    counter_.fetch_add(1);
    twist::ed::futex::WakeOne(wake_key);
  }

  void notify_one() {
    NotifyOne();
  }

  void NotifyAll() {
    auto wake_key = twist::ed::futex::PrepareWake(counter_);
    counter_.fetch_add(1);
    twist::ed::futex::WakeAll(wake_key);
  }

  void notify_all() {
    NotifyAll();
  }



 private:
  twist::ed::stdlike::atomic<uint32_t> counter_{0};
};

}  // namespace stdlike
