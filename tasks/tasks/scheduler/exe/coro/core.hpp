#pragma once

#include <sure/context.hpp>
#include <sure/stack.hpp>

#include <function2/function2.hpp>

#include <exception>

namespace exe::coro {

class Coroutine {
 public:
  using Routine = fu2::unique_function<void()>;

  explicit Coroutine(Routine routine);

  void Resume();

  // Suspend running coroutine
  static void Suspend();

  bool IsCompleted() const;

 private:
  // ???
};

}  // namespace exe::coro
