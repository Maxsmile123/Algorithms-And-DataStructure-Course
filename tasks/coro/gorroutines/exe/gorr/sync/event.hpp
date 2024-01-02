#pragma once

#include <twist/ed/stdlike/atomic.hpp>

#include <coroutine>

namespace exe::gorr {

class OneShotEvent {
 public:
  // Asynchronous
  auto Wait() {
    return std::suspend_never{};  // Not implemented
  }

  // One-shot
  void Fire() {
    // Not implemented
  }

 private:
  // ???
};

}  // namespace exe::gorr
