#include <forward_list>
#include <thread>
#include <future>

#include <fmt/core.h>
#include <gtest/gtest.h>

#include "../forward_list.hpp"

class ListTest: public testing::Test {
  protected:
    void SetUp() override {
      list.PushFront(1);
      list.PushFront(2);
      list.PushFront(3);
      list.PushFront(4);
      list.PushFront(5);
      list.PushFront(6);
      list.PushFront(7);
      assert(list.Size() == sz);
    }
  ForwardList<int> list;
  const size_t sz = 7;
};


TEST(EmptyListTest, DefaultConstructor) {
  ForwardList<int> list;
  ASSERT_TRUE(list.IsEmpty()) << "Default list isn't empty!";
}

TEST(EmptyListTest, PushFrontSimple) {
  ForwardList<int> list;
  list.PushFront(2);
  ASSERT_EQ(list.Size(), 1);
  ASSERT_EQ(list.Front(), 2);
}

TEST(EmptyListTest, PopFrontSimple) {
  ForwardList<int> list;
  list.PushFront(1);
  list.PushFront(2);
  list.PopFront();

  ASSERT_EQ(list.Size(), 1);
  ASSERT_EQ(list.Front(), 1);
}

TEST(EmptyListTest, PopFrontEmptyList) {
  ForwardList<int> list;
  EXPECT_THROW({
    list.PopFront();
  }, std::runtime_error);
}

TEST(EmptyListTest, ConstructorSizeDefaultValues) {
  ForwardList<std::string> list(5);
  ASSERT_EQ(list.Size(), 5);
  while (list.Size()) {
    ASSERT_TRUE(list.Front().empty());
    list.PopFront();
  }
}

TEST(EmptyListTest, ConstructorWithInitList) {
  ForwardList<int> list{1, 2, 3, 4, 5, 6, 7, 8};
  ASSERT_EQ(list.Size(), 8);
  int iter = 1;
  while (!list.IsEmpty()) {
    ASSERT_EQ(list.Front(), iter++);
    list.PopFront();
  }
}

TEST(EmptyListTest, Swap) {
  ForwardList<int> list;
  list.PushFront(5);

  ForwardList<int> lst;
  lst.PushFront(15);
  lst.PushFront(14);

  size_t old_mp_size = list.Size();
  size_t old_dict_size = lst.Size();

  std::swap(list, lst);

  ASSERT_EQ(lst.Size(), old_mp_size);
  ASSERT_EQ(list.Size(), old_dict_size);

  ASSERT_EQ(lst.Front(), 5);
  ASSERT_EQ(list.Front(), 15);
  list.PopFront();
  ASSERT_EQ(list.Front(), 14);
}

TEST_F(ListTest, CopyConstructor) {
  ForwardList<int> lst = list;
  ASSERT_NE(&list, &lst) << "Copy constructor must do copy!\n";
  ASSERT_EQ(list.Size(), lst.Size());
  while (!lst.IsEmpty()) {
    ASSERT_EQ(list.Front(), lst.Front());
    list.PopFront();
    ASSERT_NE(list.Front(), lst.Front());
    lst.PopFront();
  }
}

TEST_F(ListTest, CopyAssigment) {
  ForwardList<int> lst;
  lst.PushFront(4);
  list = lst;
  ASSERT_NE(&list, &lst) << "Copy assigment must do copy!\n";
  ASSERT_EQ(list.Size(), lst.Size());
  while (!lst.IsEmpty()) {
    ASSERT_EQ(list.Front(), lst.Front());
    list.PopFront();
    lst.PopFront();
  }
}

TEST_F(ListTest, SelfAssignment) {
  std::thread thread([&](){
    list = list;
  });
  auto future = std::async(std::launch::async, &std::thread::join, &thread);
  ASSERT_EQ(
    future.wait_for(std::chrono::seconds(1)),
    std::future_status::timeout
  ) << "There is infinity loop!\n";
}

TEST_F(ListTest, RangeWithIteratorPreFix) {
  ASSERT_EQ(std::distance(list.Begin(), list.End()), sz) << 
                "Distanse between begin and end iterators ins't equal size";
  int iter = 1;
  for (auto it = list.Begin(); it != list.End(); ++it) {
    ASSERT_EQ(*it, iter++);
  }
}

TEST_F(ListTest, RangeWithIteratorPostFix) {
  ASSERT_EQ(std::distance(list.Begin(), list.End()), sz) << 
                "Distanse between begin and end iterators ins't equal size";
  int iter = 1;
  for (auto it = list.Begin(); it != list.End(); it++) {
    ASSERT_EQ(*it, iter++);
  }
}

TEST_F(ListTest, EraseBegin) {
  int second_value = *(list.Begin()++);
  list.EraseAfter(list.Begin());
  ASSERT_EQ(list.Size(), sz - 1);
  ASSERT_NE(*(list.Begin()++), second_value);
}

TEST_F(ListTest, EraseMedium) {
  auto it = list.Begin();
  std::advance(it, list.Size() / 2);
  list.EraseAfter(it);
  ASSERT_EQ(list.Size(), sz - 1);
  for (auto it = list.Begin(); it != list.End(); ++it) {
    ASSERT_NE(*it, 4);
  }
}

TEST_F(ListTest, InsertMedium) {
  auto it = list.Begin();
  std::advance(it, list.Size() / 2);
  list.InsertAfter(it, 4);
  ASSERT_EQ(list.Size(), sz + 1);
  it = list.Begin();
  std::advance(it, list.Size() / 2);
  ASSERT_EQ(*it, 4);
}

TEST_F(ListTest, Clear) {
  list.Clear();
  ASSERT_TRUE(list.IsEmpty());
  ASSERT_EQ(list.Size(), 0);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}