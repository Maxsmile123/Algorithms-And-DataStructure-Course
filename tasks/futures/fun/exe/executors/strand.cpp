#include <exe/executors/strand.hpp>

namespace exe::executors {

Strand::Strand(IExecutor& /*underlying*/) {
}

void Strand::Submit(Task /*task*/) {
  // Not implemented
}

}  // namespace exe::executors
