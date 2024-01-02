#pragma once

#include "new.hpp"

#include <wheels/core/assert.hpp>

struct AllocationGuard {
  AllocationGuard()
    : before_(AllocationCount()) {
  }

  ~AllocationGuard() {
    size_t after = AllocationCount();
    WHEELS_VERIFY(before_ == after, "Allocations!");
  }

  size_t before_;
};
