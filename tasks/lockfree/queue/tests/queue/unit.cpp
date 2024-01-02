#include "../../lock_free_queue.hpp"

#include <wheels/test/framework.hpp>

#include <string>

TEST_SUITE(LockFreeQueue) {
  SIMPLE_TEST(JustWorks) {
    LockFreeQueue<std::string> queue;

    queue.Push("Payload");
    auto item = queue.TryPop();
    ASSERT_TRUE(item);
    ASSERT_EQ(*item, "Payload");

    auto empty = queue.TryPop();
    ASSERT_FALSE(empty);
  }

  SIMPLE_TEST(FIFO) {
    LockFreeQueue<int> queue;

    queue.Push(1);
    queue.Push(2);
    queue.Push(3);

    ASSERT_EQ(*queue.TryPop(), 1);
    ASSERT_EQ(*queue.TryPop(), 2);
    ASSERT_EQ(*queue.TryPop(), 3);

    ASSERT_FALSE(queue.TryPop());
  }

  SIMPLE_TEST(Dtor) {
    {
      LockFreeQueue<std::string> queue;
      queue.Push("Hello");
      queue.Push("World");
    }
  }

  struct MoveOnly {
    explicit MoveOnly(int v) : value(v) {
    }

    // Non-movable
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;

    // Movable
    MoveOnly(MoveOnly&&) = default;
    MoveOnly& operator=(MoveOnly&&) = default;

    int value;
  };

  SIMPLE_TEST(MoveOnly) {
    LockFreeQueue<MoveOnly> queue;
    queue.Push(MoveOnly(1));
    ASSERT_TRUE(queue.TryPop());
  }

  SIMPLE_TEST(TwoQueues) {
    LockFreeQueue<int> queue_1;
    LockFreeQueue<int> queue_2;

    queue_1.Push(3);
    queue_2.Push(11);
    ASSERT_EQ(*queue_1.TryPop(), 3);
    ASSERT_EQ(*queue_2.TryPop(), 11);
  }
}

RUN_ALL_TESTS()
