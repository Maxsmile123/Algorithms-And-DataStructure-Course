#pragma once

#include <cstdint>

////////////////////////////////////////////////////////////////////////////////

using Int64 = std::int64_t;
using AtomicInt64 = volatile Int64;

////////////////////////////////////////////////////////////////////////////////

// Atomic operations

// Load
// Atomically loads and returns the current value of the atomic variable

extern "C" Int64 AtomicLoadRelaxed(AtomicInt64* cell);
extern "C" Int64 AtomicLoadAcquire(AtomicInt64* cell);
extern "C" Int64 AtomicLoadSeqCst(AtomicInt64* cell);


// Store
// Atomically stores 'value' to memory location `cell`

extern "C" void AtomicStoreRelaxed(AtomicInt64* cell, Int64 value);
extern "C" void AtomicStoreRelease(AtomicInt64* cell, Int64 value);
extern "C" void AtomicStoreSeqCst(AtomicInt64* cell, Int64 value);

// Exchange
// Atomically replaces content of memory location `cell` with `value`,
// returns content of the location before the call

extern "C" Int64 AtomicExchangeSeqCst(AtomicInt64* cell, Int64 value);
