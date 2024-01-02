#pragma once

#include <cstdint>

////////////////////////////////////////////////////////////////////////////////

using Int64 = std::int64_t;
using AtomicInt64 = volatile Int64;

////////////////////////////////////////////////////////////////////////////////

// Atomic operations

// Atomically loads and returns the current value of the atomic variable
extern "C" Int64 AtomicLoad(AtomicInt64* cell);

// Atomically stores 'value' to memory location 'cell'
extern "C" void AtomicStore(AtomicInt64* cell, Int64 value);

// Atomically replaces content of memory location `cell` with `value`,
// returns content of the location before the call
extern "C" Int64 AtomicExchange(AtomicInt64* cell, Int64 value);
