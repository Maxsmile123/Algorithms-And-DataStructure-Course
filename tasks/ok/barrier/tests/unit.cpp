#include "../cyclic_barrier.hpp"

#include <wheels/test/framework.hpp>

#include <thread>

TEST_SUITE(CyclicBarrier) {
  SIMPLE_TEST(OneThread) {
    CyclicBarrier barrier{1};

    for (size_t i = 0; i < 10; ++i) {
      barrier.ArriveAndWait();
    }
  }

  SIMPLE_TEST(TwoThreads) {
    CyclicBarrier barrier{2};

    int my = 0;
    int that = 0;

    std::thread that_thread([&]() {
      that = 1;
      barrier.ArriveAndWait();
      ASSERT_EQ(my, 1);
      barrier.ArriveAndWait();
      that = 2;
      barrier.ArriveAndWait();
      ASSERT_EQ(my, 2);
    });

    my = 1;
    barrier.ArriveAndWait();
    ASSERT_EQ(that, 1);
    barrier.ArriveAndWait();
    my = 2;
    barrier.ArriveAndWait();
    ASSERT_EQ(that, 2);

    that_thread.join();
  }

  SIMPLE_TEST(Runners) {
    static const size_t kThreads = 10;
    CyclicBarrier barrier{kThreads};

    static const size_t kIterations = 256;

    std::vector<std::thread> runners;

    for (size_t i = 0; i < kThreads; ++i) {
      runners.emplace_back([&barrier]() {
        for (size_t i = 0; i < kIterations; ++i) {
          barrier.ArriveAndWait();
        }
      });
    }

    for (auto& runner : runners) {
      runner.join();
    }
  }
}

RUN_ALL_TESTS()
