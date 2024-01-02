#pragma once

template <typename Mutex>
class LockGuard {
 public:
  explicit LockGuard(Mutex&) {
    // Not implemented
  }

  // Non-copyable
  LockGuard(const LockGuard&) = delete;
  LockGuard& operator=(const LockGuard&) = delete;

  // Non-movable
  LockGuard(LockGuard&&) = delete;
  LockGuard& operator=(LockGuard&&) = delete;

  // Your code goes here
};
