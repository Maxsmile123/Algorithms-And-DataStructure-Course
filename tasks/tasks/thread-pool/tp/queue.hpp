#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>


#include <deque>
#include <optional>
#include <iostream>
#include <vector>


namespace tp {
// Unbounded Blocking Multi-Producer/Multi-Consumer (MPMC) Queue

template <typename T>
class UnboundedBlockingQueue {
 public:

  bool Put(T value) {
    std::lock_guard<twist::ed::stdlike::mutex> lock(mutex_);
    if (is_closed_.load()) {
      return false;
    }

    was_empty_.store(buffer_.empty());

    buffer_.push_back(std::move(value));
    if (was_empty_.load()) {
      no_empty_.notify_one();
    }
    return true;
  }

  std::optional<T> Take() {
    std::unique_lock<twist::ed::stdlike::mutex> lock(mutex_);

    while (buffer_.empty() && !is_closed_.load()) {
      no_empty_.wait(lock);
    }

    if (is_closed_.load() && buffer_.empty()) {
      return std::nullopt;
    }

    auto value = std::move(buffer_.front());
    buffer_.pop_front();
    return std::move(value);
  }

  void Close() {
    std::unique_lock<twist::ed::stdlike::mutex> lock(mutex_);
    is_closed_.store(true);
    no_empty_.notify_all();
  }

 private:
  std::deque<T> buffer_;
  twist::ed::stdlike::condition_variable no_empty_;
  twist::ed::stdlike::mutex mutex_;
  twist::ed::stdlike::atomic<bool> is_closed_{false};
  twist::ed::stdlike::atomic<bool> was_empty_{true};
};

}
