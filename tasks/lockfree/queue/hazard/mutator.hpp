#pragma once

#include <hazard/fwd.hpp>

#include <twist/ed/stdlike/atomic.hpp>

#include <cstdlib>

namespace hazard {

class Mutator {
  template <typename T>
  using AtomicPtr = twist::ed::stdlike::atomic<T*>;

 public:
  Mutator(Manager* /*manager*/ /*, ???*/) {
  }

  template <typename T>
  T* Protect(size_t /*index*/, AtomicPtr<T>& /*ptr*/) {
    // Not implemented
  }

  template <typename T>
  void Announce(size_t /*index*/, T* /*ptr*/) {
    // Not implemented
  }

  template <typename T>
  void Retire(T* /*ptr*/) {
    // Not implemented
  }

  void Clear() {
    // Not implemented
  }

  ~Mutator() {
    Clear();
  }
};

}  // namespace hazard
