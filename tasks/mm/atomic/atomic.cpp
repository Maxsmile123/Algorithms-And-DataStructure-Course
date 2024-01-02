#include "atomic.hpp"

namespace stdlike {

Atomic::Value Atomic::Load(MemoryOrder mo) {
  switch (mo) {
    case MemoryOrder::Relaxed:
      return AtomicLoadRelaxed(&cell_);
    case MemoryOrder::Acquire:
      return AtomicLoadAcquire(&cell_);
    case MemoryOrder::SeqCst:
      return AtomicLoadSeqCst(&cell_);
    default:
      return 0;  // UB
  }
}

void Atomic::Store(Value value, MemoryOrder mo) {
  switch (mo) {
    case MemoryOrder::Relaxed:
      AtomicStoreRelaxed(&cell_, value);
      break;
    case MemoryOrder::Release:
      AtomicStoreRelease(&cell_, value);
      break;
    case MemoryOrder::SeqCst:
      AtomicStoreSeqCst(&cell_, value);
      break;
    default:
      break;  // UB
  }
}

Atomic::Value Atomic::Exchange(Value new_value, MemoryOrder /*mo*/) {
  return AtomicExchangeSeqCst(&cell_, new_value);
}

}  // namespace stdlike
