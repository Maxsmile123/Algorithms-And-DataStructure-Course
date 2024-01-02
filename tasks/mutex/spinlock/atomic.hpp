#pragma once

#include "atomic_ops.hpp"

namespace stdlike {

// ~ std::atomic<int64_t>

class Atomic {
  using Impl = AtomicInt64;

 public:
  // Underlying value type
  using Value = Int64;

 public:
  explicit Atomic(Value initial_value = 0) : cell_(initial_value) {
  }

  // Non-copyable
  Atomic(const Atomic&) = delete;
  Atomic& operator=(const Atomic&) = delete;

  // Non-movable
  Atomic(Atomic&&) = delete;
  Atomic& operator=(Atomic&&) = delete;

  void Store(Value value) {
    AtomicStore(&cell_, value);
  }

  Value Load() {
    return AtomicLoad(&cell_);
  }

  Value Exchange(Value new_value) {
    return AtomicExchange(&cell_, new_value);
  }

 private:
  Impl cell_;
};

}  // namespace stdlike
