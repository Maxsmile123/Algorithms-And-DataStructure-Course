#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/futex.hpp>

#include <cstdlib>

namespace exe::threads {

class WaitGroup {
 public:
  // += count
  void Add(size_t /*count*/) {
    // Not implemented
  }

  // -= 1
  void Done() {
    // Not implemented
  }

  // == 0
  void Wait() {
    std::abort();  // Not implemented
  }
};

}  // namespace exe::threads
