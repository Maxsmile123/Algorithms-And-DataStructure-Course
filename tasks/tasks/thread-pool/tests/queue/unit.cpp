#include <tp/queue.hpp>

#include <wheels/test/framework.hpp>

#include <wheels/test/util/cpu_timer.hpp>

#include <string>
#include <thread>

using wheels::ProcessCPUTimer;
using wheels::ThreadCPUTimer;

template <typename T>
using Queue = tp::UnboundedBlockingQueue<T>;

TEST_SUITE(BlockingQueue) {
  SIMPLE_TEST(JustWorks) {
    Queue<int> queue;

    ASSERT_TRUE(queue.Put(7));

    auto value = queue.Take();
    ASSERT_TRUE(value);
    ASSERT_EQ(*value, 7);

    queue.Close();
    ASSERT_FALSE(queue.Take());
  }

  SIMPLE_TEST(Fifo) {
    Queue<int> queue;
    queue.Put(1);
    queue.Put(2);
    queue.Put(3);

    ASSERT_EQ(*queue.Take(), 1);
    ASSERT_EQ(*queue.Take(), 2);
    ASSERT_EQ(*queue.Take(), 3);
  }

  SIMPLE_TEST(Close) {
    Queue<std::string> queue;

    queue.Put("Hello");
    queue.Put(",");
    queue.Put("World");

    queue.Close();

    ASSERT_FALSE(queue.Put("!"));

    ASSERT_EQ(*queue.Take(), "Hello");
    ASSERT_EQ(*queue.Take(), ",");
    ASSERT_EQ(*queue.Take(), "World");
    ASSERT_FALSE(queue.Take());
  }

  struct MoveOnly {
    MoveOnly() = default;

    MoveOnly(const MoveOnly& that) = delete;
    MoveOnly& operator=(const MoveOnly& that) = delete;

    MoveOnly(MoveOnly&& that) = default;
    MoveOnly& operator=(MoveOnly&& that) = default;
  };

  SIMPLE_TEST(MoveOnly) {
    Queue<MoveOnly> queue;

    queue.Put(MoveOnly{});
    ASSERT_TRUE(queue.Take().has_value());
  }

  SIMPLE_TEST(BlockingTake) {
    Queue<int> queue;

    std::thread producer([&]() {
      std::this_thread::sleep_for(1s);
      queue.Put(7);
    });

    ThreadCPUTimer thread_cpu_timer;

    auto value = queue.Take();

    auto spent = thread_cpu_timer.Spent();

    ASSERT_TRUE(value);
    ASSERT_EQ(*value, 7);
    ASSERT_TRUE(spent < 100ms);

    producer.join();
  }

  SIMPLE_TEST(BlockingTake2) {
    Queue<int> queue;

    std::thread producer([&]() {
      std::this_thread::sleep_for(1s);
      queue.Close();
    });

    ThreadCPUTimer thread_cpu_timer;

    auto value = queue.Take();

    auto spent = thread_cpu_timer.Spent();

    ASSERT_FALSE(value);
    ASSERT_TRUE(spent < 100ms);

    producer.join();
  }

  SIMPLE_TEST(UnblockConsumers) {
    Queue<int> queue;

    // Consumers

    std::thread consumer1([&]() {
      queue.Take();
    });

    std::thread consumer2([&]() {
      queue.Take();
    });

    // Producer
    std::this_thread::sleep_for(100ms);
    queue.Close();

    consumer1.join();
    consumer2.join();
  }

  SIMPLE_TEST(ProducerConsumer) {
    Queue<int> queue;

    ProcessCPUTimer process_cpu_timer;

    std::thread producer([&]() {
      // Producer
      for (int i = 0; i < 10; ++i) {
        queue.Put(i);
        std::this_thread::sleep_for(100ms);
      }
      queue.Close();
    });

    // Consumer

    for (int i = 0; i < 10; ++i) {
      auto value = queue.Take();
      ASSERT_TRUE(value);
      ASSERT_EQ(*value, i);
    }

    ASSERT_FALSE(queue.Take());

    producer.join();

    ASSERT_TRUE(process_cpu_timer.Spent() < 100ms);
  }
}

RUN_ALL_TESTS()
