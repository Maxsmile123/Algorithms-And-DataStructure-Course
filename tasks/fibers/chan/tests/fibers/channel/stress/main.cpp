#include <exe/executors/thread_pool.hpp>

#include <exe/fibers/core/api.hpp>
#include <exe/fibers/sync/channel.hpp>

#include <wheels/test/framework.hpp>
#include <twist/test/budget.hpp>

#include <course/twist/test.hpp>

#include <random>

using namespace exe;

//////////////////////////////////////////////////////////////////////

class ChannelTester {
  class StartLatch {
   public:
    void Release() {
      started_.store(true);
    }

    void Await() {
      while (!started_.load()) {
        fibers::self::Yield();
      }
    }

   private:
    std::atomic<bool> started_{false};
  };

 public:
  explicit ChannelTester(size_t buffer_size, size_t producers, size_t consumers)
      : producers_(producers), consumers_(consumers), ints_(buffer_size) {
  }

  void RunTest() {
    // Producers

    producers_left_.store(producers_);

    for (size_t i = 0; i < producers_; ++i) {
      fibers_.fetch_add(1);
      fibers::Go(pool_, [i, this]() {
        Producer(i);
      });
    }

    // Consumers

    for (size_t j = 0; j < consumers_; ++j) {
      fibers_.fetch_add(1);
      fibers::Go(pool_, [j, this]() {
        Consumer(j);
      });
    }

    start_latch_.Release();

    pool_.WaitIdle();

    std::cout << "Checksum: " << checksum_.load() << std::endl;
    std::cout << "Sends: " << sends_.load() << std::endl;

    ASSERT_EQ(fibers_.load(), 0);
    ASSERT_EQ(checksum_.load(), 0);

    pool_.Stop();
  }

 private:
  void Send(int64_t value, size_t iter) {
    if (iter % 4 == 0) {
      if (ints_.TrySend(value)) {
        return;
      }
    }
    (void)iter;
    ints_.Send(value);
  }

  void Producer(size_t index) {
    start_latch_.Await();

    std::mt19937 twister{(uint32_t)index};

    size_t iter = 0;

    while (twist::test::KeepRunning()) {
      ++iter;

      uint32_t value = twister();

      Send((int64_t)value, iter);

      checksum_.fetch_xor(value, std::memory_order_relaxed);
      sends_.fetch_add(1, std::memory_order_relaxed);

      if (iter % 7 == 0) {
        fibers::self::Yield();
      }
    }

    if (producers_left_.fetch_sub(1) == 1) {
      // Last producer
      for (size_t j = 0; j < consumers_; ++j) {
        ints_.Send(-1);  // Poison pill
      }
    }

    fibers_.fetch_sub(1);
  }

  int64_t Receive(size_t iter) {
    if (iter % 7 == 0) {
      if (auto value = ints_.TryReceive()) {
        return *value;
      }
    }
    (void)iter;
    return ints_.Receive();
  }

  void Consumer(size_t /*index*/) {
    start_latch_.Await();

    size_t iter = 0;

    while (true) {
      ++iter;

      int64_t value = Receive(iter);
      if (value == -1) {
        break;
      }

      checksum_.fetch_xor((uint32_t)value, std::memory_order_relaxed);

      if (iter % 7 == 0) {
        fibers::self::Yield();
      }
    }

    fibers_.fetch_sub(1);
  }

 private:
  const size_t producers_;
  const size_t consumers_;

  executors::ThreadPool pool_{4};
  fibers::Channel<int64_t> ints_;

  std::atomic<uint64_t> checksum_{0};
  std::atomic<size_t> sends_{0};
  std::atomic<size_t> producers_left_{0};

  std::atomic<size_t> fibers_{0};

  StartLatch start_latch_;
};

//////////////////////////////////////////////////////////////////////

void StressTest(size_t buffer, size_t producers, size_t consumers) {
  ChannelTester tester{buffer, producers, consumers};
  tester.RunTest();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(Channel) {
  TWIST_TEST(Stress1, 5s) {
    StressTest(1, 2, 5);
  }

  TWIST_TEST(Stress2, 5s) {
    StressTest(1, 5, 2);
  }

  TWIST_TEST(Stress3, 5s) {
    StressTest(3, 5, 7);
  }

  TWIST_TEST(Stress4, 5s) {
    StressTest(3, 8, 6);
  }

  TWIST_TEST(Stress5, 5s) {
    StressTest(11, 15, 12);
  }

  TWIST_TEST(Stress6, 5s) {
    StressTest(11, 14, 18);
  }
}

RUN_ALL_TESTS()
