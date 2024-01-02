#pragma once

#include "atomic.hpp"

#include <twist/ed/wait/spin.hpp>

// Naive Test-and-Set (TAS) spinlock

class TASSpinLock {
 public:
  void Lock() {
    while (locked_.Exchange(1) == 1) {
      twist::ed::CpuRelax();
    }
  }

  bool TryLock() {
    return false;  // Not implemented
  }

  void Unlock() {
    locked_.Store(0);
  }

 private:
  stdlike::Atomic locked_{0};
};
