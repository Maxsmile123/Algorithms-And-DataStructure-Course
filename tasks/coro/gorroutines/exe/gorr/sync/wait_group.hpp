#pragma once

#include <exe/gorr/sync/event.hpp>

#include <twist/ed/stdlike/atomic.hpp>

namespace exe::gorr {

class WaitGroup {
 public:
  void Add(size_t /*count*/) {
    // Not implemented
  }

  void Done() {
    // Not implemented
  }

  // Asynchronous
  auto Wait() {
    return std::suspend_never{};  // Not implemented
  }

 private:
  // ???
};

}  // namespace exe::gorr
