#pragma once

#include "shared_ptr.hpp"

#include <twist/ed/stdlike/atomic.hpp>

#include <optional>

// Treiber unbounded lock-free stack
// https://en.wikipedia.org/wiki/Treiber_stack

template <typename T>
class LockFreeStack {
  struct Node {
    explicit Node(T v) : value(std::move(v)) {
    }

    T value;
    SharedPtr<Node> next;
  };

 public:
  void Push(T value) {
    auto new_node = MakeShared<Node>(std::move(value));
    new_node->next = top_.Load();
    while (!top_.CompareExchangeWeak(new_node->next, new_node)) {
      ;
    }
  }

  std::optional<T> TryPop() {
    while (true) {
      SharedPtr<Node> curr_top = top_.Load();
      if (!curr_top) {
        return std::nullopt;
      }
      if (top_.CompareExchangeWeak(curr_top, curr_top->next)) {
        return std::move(curr_top->value);
      }
    }
  }

 private:
  AtomicSharedPtr<Node> top_;
};
