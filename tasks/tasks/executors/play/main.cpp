#include <exe/executors/thread_pool.hpp>
#include <exe/executors/strand.hpp>
#include <exe/executors/submit.hpp>

#include <fmt/core.h>

using namespace exe::executors;

int main() {
  ThreadPool pool{4};
  Strand strand{pool};

  pool.Start();

  size_t cs = 0;

  for (size_t i = 0; i < 12345; ++i) {
    Submit(strand, [&cs] {
      ++cs;  // <-- Critical section
    });
  }

  pool.WaitIdle();

  fmt::println("# critical sections: {}", cs);

  pool.Stop();
  return 0;
}
