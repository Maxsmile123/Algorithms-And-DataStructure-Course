#pragma once

#include "tagged_semaphore.hpp"

#include <deque>
#include <vector>

// Bounded Blocking Multi-Producer/Multi-Consumer (MPMC) Queue

template <typename T>
class BoundedBlockingQueue {
 public:
  explicit BoundedBlockingQueue(size_t cap): put_(cap), take_(0){
  }

  void Put(T value) {
    auto token = put_.Acquire();
    buffer_.push_back(std::move(value));
    take_.Release(std::move(token));
  }

  T Take() {
    auto token = take_.Acquire();
    auto value = std::move(buffer_.front());
    buffer_.pop_front();
    put_.Release(std::move(token));
    return std::move(value);
  }

 private:
  struct SharedTag {};

 private:
  std::deque<T> buffer_;
  TaggedSemaphore<SharedTag> put_;
  TaggedSemaphore<SharedTag> take_;
};
