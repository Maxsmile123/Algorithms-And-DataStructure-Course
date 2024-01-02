#pragma once

#include <sure/context.hpp>
#include <sure/stack.hpp>
#include <sure/trampoline.hpp>

#include <function2/function2.hpp>

#include <exception>

// Simple stackful coroutine

const size_t STACKSIZE = 4194304; // 4 MB

class Coroutine: private sure::ITrampoline {
 public:
  using Routine = fu2::unique_function<void()>;

  explicit Coroutine(Routine routine);

  void Resume();

  // Suspend running coroutine
  static void Suspend();

  bool IsCompleted() const;

 private:
  [[noreturn]] void Run() noexcept override final;

  void SwitchToCaller();

  Coroutine* GetCaller();

  typedef enum {
    INIT,
    RUNNING,
    FINISHED
  } State;

 private:
  Routine routine_;
  std::exception_ptr eptr_ = nullptr;
  State state_;

  Coroutine* caller;
  sure::Stack callee_stack_;
  sure::ExecutionContext callee_context_;
  sure::ExecutionContext caller_context_;
};
