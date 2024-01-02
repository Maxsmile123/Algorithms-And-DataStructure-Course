#pragma once

#include <cstdlib>
#include <memory>
#include <optional>

namespace exe::fibers {

//////////////////////////////////////////////////////////////////////

namespace detail {

template <typename X, typename Y>
class Selector;

template <typename T>
class ChannelImpl {
 public:
  explicit ChannelImpl(size_t capacity) : capacity_(capacity) {
  }

  ~ChannelImpl() {
    // Not implemented
    // Check that receivers/senders queues are empty
  }

  void Send(T /*value*/) {
    std::abort();  // Not implemented
  }

  bool TrySend(T /*value*/) {
    std::abort();  // Not implemented
  }

  T Receive() {
    std::abort();  // Not implemented
  }

  std::optional<T> TryReceive() {
    std::abort();  // Not implemented
  }

 private:
  const size_t capacity_;
  // ???
};

}  // namespace detail

//////////////////////////////////////////////////////////////////////

// Buffered Multi-Producer / Multi-Consumer Channel
// https://tour.golang.org/concurrency/3

// Does not support void type
// Use wheels::Unit instead (from <wheels/core/unit.hpp>)

template <typename T>
class Channel {
  using Impl = detail::ChannelImpl<T>;

  template <typename X, typename Y>
  friend class detail::Selector;

 public:
  // Bounded channel, `capacity` > 0
  explicit Channel(size_t capacity) : impl_(std::make_shared<Impl>(capacity)) {
  }

  // Suspending
  void Send(T value) {
    impl_->Send(std::move(value));
  }

  bool TrySend(T value) {
    return impl_->TrySend(std::move(value));
  }

  // Suspending
  T Receive() {
    return impl_->Receive();
  }

  std::optional<T> TryReceive() {
    return impl_->TryReceive();
  }

 private:
  std::shared_ptr<Impl> impl_;
};

}  // namespace exe::fibers
