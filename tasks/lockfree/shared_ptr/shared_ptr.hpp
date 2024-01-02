#pragma once

#include "atomic_stamped_ptr.hpp"

#include <twist/ed/stdlike/atomic.hpp>

//////////////////////////////////////////////////////////////////////

namespace detail {

struct SplitCount {
  int32_t transient;
  int32_t strong;  // > 0
};

static_assert(sizeof(SplitCount) == sizeof(size_t), "Not supported");

}  // namespace detail

//////////////////////////////////////////////////////////////////////

template <typename T>
class SharedPtr {
 public:
  SharedPtr() {
    // Not implemented
  }

  SharedPtr(const SharedPtr<T>& /*that*/) {
    // Not implemented
  }

  SharedPtr<T>& operator=(const SharedPtr<T>& /*that*/) {
    return *this;  // Not implemented
  }

  SharedPtr(SharedPtr<T>&& /*that*/) {
    // Not implemented
  }

  SharedPtr<T>& operator=(SharedPtr<T>&& /*that*/) {
    // Not implemented
    return *this;
  }

  T* operator->() const {
    return nullptr;  // Not implemented
  }

  T& operator*() const {
    std::abort();  // Not implemented
  }

  explicit operator bool() const {
    return true;  // Not implemented
  }

  void Reset() {
    // Not implemented
  }

  ~SharedPtr() {
    Reset();
  }

 private:
  // ???
};

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... /*args*/) {
  std::abort();  // Not implemented
}

//////////////////////////////////////////////////////////////////////

template <typename T>
class AtomicSharedPtr {
 public:
  // nullptr
  AtomicSharedPtr() {
    // Not implemented
  }

  ~AtomicSharedPtr() {
    // Not implemented
  }

  SharedPtr<T> Load() {
    std::abort();  // Not implemented
  }

  void Store(SharedPtr<T> /*target*/) {
    // Not implemented
  }

  explicit operator SharedPtr<T>() {
    return Load();
  }

  bool CompareExchangeWeak(SharedPtr<T>& /*expected*/,
                           SharedPtr<T> /*desired*/) {
    return false;  // Not implemented
  }

 private:
  // ???
};
