#include "coroutine.hpp"

#include <twist/ed/local/ptr.hpp>

#include <iostream>

#include <wheels/core/assert.hpp>
#include <wheels/core/compiler.hpp>

TWISTED_THREAD_LOCAL_PTR(Coroutine, current);

Coroutine::Coroutine(Routine routine): 
  routine_(std::move(routine)),
  callee_stack_(sure::Stack::AllocateBytes(STACKSIZE))
{
  caller = current;
  callee_context_.Setup(callee_stack_.MutView(), this);
  state_ = State::INIT;
}

void Coroutine::Resume() {
  current = this;
  this->state_ = State::RUNNING;
  caller_context_.SwitchTo(callee_context_);
  if (eptr_) {
    std::rethrow_exception(eptr_);
  }
}

void Coroutine::Suspend() {
  current->SwitchToCaller();
}

Coroutine* Coroutine::GetCaller() {
  return caller;
}

void Coroutine::SwitchToCaller() {
  current = current->GetCaller();
  callee_context_.SwitchTo(caller_context_);
}



bool Coroutine::IsCompleted() const {
  return state_ == State::FINISHED;  
}

void Coroutine::Run() noexcept {
  try {
    routine_();
  } catch(...) {
    eptr_ = std::current_exception();
  }
  this->state_ = State::FINISHED;
  current = current->GetCaller();
  callee_context_.ExitTo(caller_context_);
}
