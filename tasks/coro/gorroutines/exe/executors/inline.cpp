#include <exe/executors/inline.hpp>

namespace exe::executors {

class InlineExecutor : public IExecutor {
 public:
  // IExecutor
  void Submit(Task task) override {
    task();
  }
};

IExecutor& Inline() {
  static InlineExecutor instance;
  return instance;
}

}  // namespace exe::executors
