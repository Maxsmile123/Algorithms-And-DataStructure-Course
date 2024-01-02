#include <cstdlib>

#include <atomic>

static std::atomic<size_t> alloc_count{0};

void* operator new(size_t size) {
  alloc_count.fetch_add(1, std::memory_order::relaxed);
  return std::malloc(size);
}

size_t AllocationCount() {
  return alloc_count.load(std::memory_order::relaxed);
}
