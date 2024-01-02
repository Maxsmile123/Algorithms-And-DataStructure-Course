#include <exe/executors/thread_pool.hpp>
#include <exe/executors/strand.hpp>
#include <exe/executors/submit.hpp>

#include <exe/threads/wait_group.hpp>

#include <course/twist/test.hpp>

#include <twist/test/repeat.hpp>

#include <thread>

using namespace exe;
using namespace std::chrono_literals;

/////////////////////////////////////////////////////////////////////

class OnePassBarrier {
 public:
  explicit OnePassBarrier(size_t threads) : total_(threads) {
  }

  void Pass() {
    arrived_.fetch_add(1);
    while (arrived_.load() < total_) {
      std::this_thread::yield();
    }
  }

 private:
  const size_t total_{0};
  std::atomic<size_t> arrived_{0};
};

void MaybeAnomaly() {
  executors::ThreadPool pool{1};
  pool.Start();

  twist::test::Repeat repeat;

  while (repeat()) {
    executors::Strand strand{pool};
    OnePassBarrier barrier{2};

    threads::WaitGroup wg;
    wg.Add(2);

    executors::Submit(strand, [&wg, &barrier] {
      wg.Done();
      barrier.Pass();
    });

    barrier.Pass();

    executors::Submit(strand, [&wg] {
      wg.Done();
    });

    wg.Wait();
  }

  pool.Stop();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(Strand) {
  TWIST_TEST(MemoryOrder, 5s) {
    MaybeAnomaly();
  }
}

RUN_ALL_TESTS()
