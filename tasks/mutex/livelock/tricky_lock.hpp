#pragma once

#include <cstdlib>
#include <atomic>

// For reference only

class TrickyLock {
 public:
  void Lock() {
    while (thread_count_.fetch_add(1) > 0) {
      thread_count_.fetch_sub(1);  // Step back
    }
  }

  void Unlock() {
    thread_count_.fetch_sub(1);
  }

 private:
  std::atomic<size_t> thread_count_{0};
};
