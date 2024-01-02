#include "../semaphore.hpp"
#include "../queue.hpp"

#include <wheels/test/framework.hpp>

#include <atomic>
#include <chrono>
#include <deque>
#include <random>
#include <string>
#include <thread>

using namespace std::chrono_literals;

////////////////////////////////////////////////////////////////////////////////

TEST_SUITE(Semaphore) {
  SIMPLE_TEST(NonBlocking) {
    Semaphore semaphore(2);

    semaphore.Acquire();  // -1
    semaphore.Release();  // +1

    semaphore.Acquire();  // -1
    semaphore.Acquire();  // -1
    semaphore.Release();  // +1
    semaphore.Release();  // +1
  }

  SIMPLE_TEST(Blocking) {
    Semaphore semaphore(0);

    bool touched = false;

    std::thread touch([&]() {
      semaphore.Acquire();
      touched = true;
    });

    std::this_thread::sleep_for(250ms);

    ASSERT_FALSE(touched);

    semaphore.Release();
    touch.join();

    ASSERT_TRUE(touched);
  }

  SIMPLE_TEST(PingPong) {
    Semaphore my{1};
    Semaphore that{0};

    int step = 0;

    std::thread opponent([&]() {
      that.Acquire();
      ASSERT_EQ(step, 1);
      step = 0;
      my.Release();
    });

    my.Acquire();
    ASSERT_EQ(step, 0);
    step = 1;
    that.Release();

    my.Acquire();
    ASSERT_TRUE(step == 0);

    opponent.join();
  }
}

////////////////////////////////////////////////////////////////////////////////

TEST_SUITE(BoundedBlockingQueue) {
  SIMPLE_TEST(PutThenTake) {
    BoundedBlockingQueue<int> queue{1};
    queue.Put(42);
    ASSERT_EQ(queue.Take(), 42);
  }

  struct MoveOnly {
    MoveOnly() = default;

    MoveOnly(const MoveOnly& that) = delete;
    MoveOnly& operator=(const MoveOnly& that) = delete;

    MoveOnly(MoveOnly&& that) = default;
    MoveOnly& operator=(MoveOnly&& that) = default;
  };

  SIMPLE_TEST(MoveOnly) {
    BoundedBlockingQueue<MoveOnly> queue{1};

    queue.Put(MoveOnly{});
    queue.Take();
  }

  SIMPLE_TEST(Buffer) {
    BoundedBlockingQueue<std::string> queue{2};

    queue.Put("hello");
    queue.Put("world");

    ASSERT_EQ(queue.Take(), "hello");
    ASSERT_EQ(queue.Take(), "world");
  }

  SIMPLE_TEST(FifoSmall) {
    BoundedBlockingQueue<std::string> queue{2};

    std::thread producer([&queue]() {
      queue.Put("hello");
      queue.Put("world");
      queue.Put("!");
    });

    ASSERT_EQ(queue.Take(), "hello");
    ASSERT_EQ(queue.Take(), "world");
    ASSERT_EQ(queue.Take(), "!");

    producer.join();
  }

  SIMPLE_TEST(Fifo) {
    BoundedBlockingQueue<int> queue{3};

    static const int kItems = 1024;

    std::thread producer([&]() {
      for (int i = 0; i < kItems; ++i) {
        queue.Put(i);
      }
      queue.Put(-1);  // Poison pill
    });

    // Consumer

    for (int i = 0; i < kItems; ++i) {
      ASSERT_EQ(queue.Take(), i);
    }
    ASSERT_EQ(queue.Take(), -1);

    producer.join();
  }

  SIMPLE_TEST(Capacity) {
    BoundedBlockingQueue<int> queue{3};
    std::atomic<size_t> send_count{0};

    std::thread producer([&]() {
      for (size_t i = 0; i < 100; ++i) {
        queue.Put(i);
        send_count.store(i);
      }
      queue.Put(-1);
    });

    std::this_thread::sleep_for(100ms);

    ASSERT_TRUE(send_count.load() <= 3);

    for (size_t i = 0; i < 14; ++i) {
      (void)queue.Take();
    }

    std::this_thread::sleep_for(100ms);

    ASSERT_TRUE(send_count.load() <= 17);

    while (queue.Take() != -1) {
      // Pass
    }

    producer.join();
  }

  SIMPLE_TEST(Pill) {
    static const size_t kThreads = 10;
    BoundedBlockingQueue<int> queue{1};

    std::vector<std::thread> threads;

    std::mt19937 twister;

    for (size_t i = 0; i < kThreads; ++i) {
      threads.emplace_back([&]() {
        std::this_thread::sleep_for(1ms * (twister() % 1000));

        ASSERT_EQ(queue.Take(), -1);
        queue.Put(-1);
      });
    }

    queue.Put(-1);

    for (auto& t : threads) {
      t.join();
    }
  }
}

RUN_ALL_TESTS()
