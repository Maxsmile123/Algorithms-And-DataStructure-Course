#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/spin.hpp>

/*
 * Scalable Queue SpinLock
 *
 * Usage:
 *
 * QueueSpinLock spinlock;
 *
 * {
 *   QueueSpinLock::Guard guard(spinlock);  // <-- Acquire
 *   // <-- Critical section
 * }  // <-- Release
 *
 */

class QueueSpinLock {
 public:
  class Guard {
    friend class QueueSpinLock;

   public:
    explicit Guard(QueueSpinLock& host)
        : host_(host) {
      host_.Acquire(this);
    }

    ~Guard() {
      host_.Release(this);
    }

   private:
    QueueSpinLock& host_;
    // ???
  };

 private:
  void Acquire(Guard* /*waiter*/) {
    // Your code goes here
  }

  void Release(Guard* /*owner*/) {
    // Your code goes here
  }

 private:
  // ???
};
