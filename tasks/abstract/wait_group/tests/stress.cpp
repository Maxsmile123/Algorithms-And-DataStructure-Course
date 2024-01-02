#include "../wait_group.hpp"

#include <course/twist/test.hpp>

#include <twist/test/race.hpp>
#include <twist/test/random.hpp>

#include <array>

void StressTest() {
  WaitGroup wg;

  size_t workers = twist::test::Random(1, 5);
  size_t waiters = twist::test::Random(1, 5);

  std::array<bool, 6> work;
  work.fill(false);

  twist::test::Race race;

  wg.Add(workers);

  for (size_t i = 0; i < waiters; ++i) {
    race.Add([&] {
      wg.Wait();
      for (size_t k = 0; k < workers; ++k) {
        ASSERT_TRUE(work[k]);
      }
    });
  }

  for (size_t k = 0; k < workers; ++k) {
    race.Add([&, k] {
      work[k] = true;
      wg.Done();
    });
  }

  race.Run();
}

TEST_SUITE(WaitGroup) {
  TWIST_TEST_REPEAT(Stress, 5s) {
    StressTest();
  }
}

RUN_ALL_TESTS();
