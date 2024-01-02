#pragma once

#include "atomic_ops.hpp"

namespace stdlike {

//////////////////////////////////////////////////////////////////////

enum class MemoryOrder {
  Relaxed,
  Release,
  Acquire,
  SeqCst
};

//////////////////////////////////////////////////////////////////////

class Atomic {
  using Impl = AtomicInt64;

 public:
  // Underlying value type
  using Value = Int64;

 public:
  explicit Atomic(Value initial_value = 0)
      : cell_(initial_value) {
  }

  // Non-copyable
  Atomic(const Atomic&) = delete;
  Atomic& operator=(const Atomic&) = delete;

  // Non-movable
  Atomic(Atomic&&) = delete;
  Atomic& operator=(Atomic&&) = delete;

  // Operations

  Value Load(MemoryOrder mo = MemoryOrder::SeqCst);

  operator Value() {
    return Load();
  }

  void Store(Value value, MemoryOrder mo = MemoryOrder::SeqCst);

  Atomic& operator=(Value value) {
    Store(value);
    return *this;
  }

  Value Exchange(Value new_value, MemoryOrder mo = MemoryOrder::SeqCst);

 private:
  Impl cell_;
};

}  // namespace stdlike
