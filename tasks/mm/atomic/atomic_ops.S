#if(APPLE)
  #define FUNCTION_NAME(name) _##name
#else
  #define FUNCTION_NAME(name) name
#endif

.globl FUNCTION_NAME(AtomicLoadRelaxed)
.globl FUNCTION_NAME(AtomicLoadAcquire)
.globl FUNCTION_NAME(AtomicLoadSeqCst)

.globl FUNCTION_NAME(AtomicStoreRelaxed)
.globl FUNCTION_NAME(AtomicStoreRelease)
.globl FUNCTION_NAME(AtomicStoreSeqCst)

.globl FUNCTION_NAME(AtomicExchangeSeqCst)

# Implementation

# Load

FUNCTION_NAME(AtomicLoadRelaxed):
    movq (%rdi), %rax
    retq

FUNCTION_NAME(AtomicLoadAcquire):
    movq (%rdi), %rax
    retq

FUNCTION_NAME(AtomicLoadSeqCst):
    movq (%rdi), %rax
    retq

# Store

FUNCTION_NAME(AtomicStoreRelaxed):
    movq %rsi, (%rdi)
    retq

FUNCTION_NAME(AtomicStoreRelease):
    movq %rsi, (%rdi)
    retq

FUNCTION_NAME(AtomicStoreSeqCst):
    movq %rsi, (%rdi)
    retq

# Exchange

FUNCTION_NAME(AtomicExchangeSeqCst):
    movq (%rdi), %rax  # Load
    movq %rsi, (%rdi)  # Store
    retq
