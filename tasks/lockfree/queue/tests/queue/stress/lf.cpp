#include "../../../lock_free_queue.hpp"

#include <course/twist/test.hpp>

#include <twist/test/budget.hpp>
#include <twist/test/lockfree.hpp>
#include <twist/test/race.hpp>
#include <twist/test/random.hpp>

#include <atomic>
#include <iostream>
#include <limits>

//////////////////////////////////////////////////////////////////////

struct Widget {
  size_t data;

  explicit Widget(size_t d)
      : data(d) {
  }
};

//////////////////////////////////////////////////////////////////////

void StressTest(size_t threads, size_t batch_size_limit) {
  twist::test::SetLockFreeAdversary();

  twist::test::ReportProgressFor<LockFreeQueue<Widget>> queue;

  std::atomic<size_t> ops{0};
  std::atomic<size_t> pushed{0};
  std::atomic<size_t> popped{0};

  twist::test::Race race;

  for (size_t i = 0; i < threads; ++i) {
    race.Add([&] {
      twist::test::EnablePark lf_here;

      for (twist::test::TimeBudget budget; budget; ) {
        size_t batch_size = twist::test::Random(1, batch_size_limit + 1);

        // Push

        for (size_t j = 0; j < batch_size; ++j) {
          Widget w{twist::test::Random(1000007)};

          pushed.fetch_add(w.data);
          queue->Push(std::move(w));
          ++ops;
        }

        // Pop

        for (size_t j = 0; j < batch_size; ++j) {
          auto w = queue->TryPop();
          ASSERT_TRUE(w);
          popped.fetch_add(w->data);
        }
      }
    });
  }

  race.Run();

  std::cout << "Operations: " << ops.load() << std::endl;
  std::cout << "Pushed: " << pushed.load() << std::endl;
  std::cout << "Popped: " << popped.load() << std::endl;

  ASSERT_EQ(pushed.load(), popped.load());

  // Queue is empty
  ASSERT_FALSE(queue->TryPop());
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(LockFreeQueue) {
  TWIST_TEST(Stress1, 5s) {
    StressTest(/*threads=*/2, /*batch_size_limit=*/2);
  }

  TWIST_TEST(Stress2, 5s) {
    StressTest(/*threads=*/5, /*batch_size_limit=*/1);
  }

  TWIST_TEST(Stress3, 5s) {
    StressTest(/*threads=*/5, /*batch_size_limit=*/3);
  }

  TWIST_TEST(Stress4, 5s) {
    StressTest(/*threads=*/5, /*batch_size_limit=*/5);
  }
}

RUN_ALL_TESTS()
