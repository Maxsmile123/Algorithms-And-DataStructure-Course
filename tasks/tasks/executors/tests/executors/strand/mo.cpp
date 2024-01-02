#include <exe/executors/thread_pool.hpp>
#include <exe/executors/strand.hpp>
#include <exe/executors/submit.hpp>

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

  for (twist::test::Repeat repeat; repeat.Test(); ) {
    executors::Strand strand{pool};
    OnePassBarrier barrier{2};

    size_t done = 0;

    executors::Submit(strand, [&done, &barrier] {
      ++done;
      barrier.Pass();
    });

    barrier.Pass();

    executors::Submit(strand, [&done] {
      ++done;
    });

    pool.WaitIdle();

    ASSERT_EQ(done, 2);
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
