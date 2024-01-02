#include <exe/coroutine/impl.hpp>

#include <sure/stack.hpp>

namespace exe::coroutine {

// Standalone coroutine

class Coroutine {
 public:
  explicit Coroutine(Routine routine);

  void Resume();

  // Suspend current coroutine
  static void Suspend();

  bool IsCompleted() {
    return impl_.IsCompleted();
  }

 private:
  static sure::Stack AllocateStack();

 private:
  sure::Stack stack_;
  CoroutineImpl impl_;
};

}  // namespace exe::coroutine
