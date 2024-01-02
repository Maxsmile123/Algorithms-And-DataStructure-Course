#pragma once

#include "semaphore.hpp"

#include <cassert>

template <class Tag>
class TaggedSemaphore {
 public:
  // ~ Linear
  class Token {
    friend class TaggedSemaphore;

   public:
    ~Token() {
      assert(!valid_);
    }

    // Non-copyable
    Token(const Token&) = delete;
    Token& operator=(const Token&) = delete;

    // Movable

    Token(Token&& that) {
      that.Invalidate();
    }

    Token& operator=(Token&&) = delete;

   private:
    Token() = default;

    void Invalidate() {
      assert(valid_);
      valid_ = false;
    }

   private:
    bool valid_{true};
  };

 public:
  explicit TaggedSemaphore(size_t tokens)
      : impl_(tokens) {
  }

  Token Acquire() {
    impl_.Acquire();
    return Token{};
  }

  void Release(Token&& token) {
    impl_.Release();
    token.Invalidate();
  }

 private:
  Semaphore impl_;
};
