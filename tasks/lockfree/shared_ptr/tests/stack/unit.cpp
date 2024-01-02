#include "../../lock_free_stack.hpp"

#include <wheels/test/framework.hpp>

#include <string>

TEST_SUITE(LockFreeStack) {
  SIMPLE_TEST(JustWorks) {
    LockFreeStack<std::string> stack;

    stack.Push("Data");
    auto item = stack.TryPop();
    ASSERT_TRUE(item);
    ASSERT_EQ(*item, "Data");

    auto empty = stack.TryPop();
    ASSERT_FALSE(empty);
  }

  SIMPLE_TEST(LIFO) {
    LockFreeStack<int> stack;

    stack.Push(1);
    stack.Push(2);
    stack.Push(3);

    ASSERT_EQ(*stack.TryPop(), 3);
    ASSERT_EQ(*stack.TryPop(), 2);
    ASSERT_EQ(*stack.TryPop(), 1);

    ASSERT_FALSE(stack.TryPop());
  }

  SIMPLE_TEST(Dtor) {
    LockFreeStack<std::string> stack;
    stack.Push("Hello");
    stack.Push("World");
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
    LockFreeStack<MoveOnly> stack;
    stack.Push(MoveOnly(1));
    ASSERT_TRUE(stack.TryPop());
  }

  SIMPLE_TEST(TwoStacks) {
    LockFreeStack<int> stack_1;
    LockFreeStack<int> stack_2;

    stack_1.Push(3);
    stack_2.Push(11);
    ASSERT_EQ(*stack_1.TryPop(), 3);
    ASSERT_EQ(*stack_2.TryPop(), 11);
  }
}

RUN_ALL_TESTS()
