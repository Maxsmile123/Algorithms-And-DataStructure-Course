#pragma once

#include <twist/ed/stdlike/atomic.hpp>

namespace exe::fibers {

// https://gobyexample.com/waitgroups

class WaitGroup {
 public:
  void Add(size_t /*count*/) {
    // Not implemented
  }

  void Done() {
    // Not implemented
  }

  void Wait() {
    // Not implemented
  }

 private:
  // ???
};

}  // namespace exe::fibers
