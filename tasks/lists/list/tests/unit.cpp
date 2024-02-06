#include <list>
#include <thread>
#include <future>

#include <fmt/core.h>
#include <gtest/gtest.h>

#include "../list.hpp"

class ListTest: public testing::Test {
  protected:
    void SetUp() override {
      list.PushBack(1);
      list.PushBack(2);
      list.PushBack(3);
      list.PushBack(4);
      list.PushBack(5);
      list.PushBack(6);
      list.PushBack(7);
      assert(list.Size() == sz);
    }
  List<int> list;
  const size_t sz = 7;
};


TEST(EmptyListTest, DefaultConstructor) {
  List<int> list;
  ASSERT_TRUE(list.IsEmpty()) << "Default list isn't empty!";
}

TEST(EmptyListTest, PushBackSimple) {
  List<int> list;
  list.PushBack(1);
  ASSERT_EQ(list.Size(), 1);

  ASSERT_EQ(list.Front(), list.Back());
  ASSERT_EQ(list.Front(), 1);
}

TEST(EmptyListTest, PushFrontSimple) {
  List<int> list;
  list.PushBack(1);
  list.PushFront(2);
  ASSERT_EQ(list.Size(), 2);
  ASSERT_EQ(list.Front(), 2);
}

TEST(EmptyListTest, PopFrontSimple) {
  List<int> list;
  list.PushBack(1);
  list.PushFront(2);
  list.PopFront();

  ASSERT_EQ(list.Size(), 1);
  ASSERT_EQ(list.Front(), list.Back());
  ASSERT_EQ(list.Front(), 1);
}

TEST(EmptyListTest, PopBackSimple) {
  List<int> list;
  list.PushBack(1);
  list.PushFront(2);
  list.PopBack();

  ASSERT_EQ(list.Size(), 1);
  ASSERT_EQ(list.Front(), list.Back());
  ASSERT_EQ(list.Front(), 2);
}

TEST(EmptyListTest, PopBackEmptyList) {
  List<int> list;
  EXPECT_THROW({
    list.PopBack();
  }, std::runtime_error);
}

TEST(EmptyListTest, PopFrontEmptyList) {
  List<int> list;
  EXPECT_THROW({
    list.PopFront();
  }, std::runtime_error);
}

TEST(EmptyListTest, ConstructorSizeDefaultValues) {
  List<std::string> list(5);
  ASSERT_EQ(list.Size(), 5);
  while (list.Size()) {
    ASSERT_TRUE(list.Front().empty());
    list.PopFront();
  }
}

TEST(EmptyListTest, ConstructorWithInitList) {
  List<int> list{1, 2, 3, 4, 5, 6, 7, 8};
  ASSERT_EQ(list.Size(), 8);
  int iter = 1;
  while (!list.IsEmpty()) {
    ASSERT_EQ(list.Front(), iter++);
    list.PopFront();
  }
}

TEST(EmptyListTest, Swap) {
  List<int> list;
  list.PushBack(5);

  List<int> lst;
  lst.PushBack(15);
  lst.PushBack(14);

  size_t old_mp_size = list.Size();
  size_t old_dict_size = lst.Size();

  std::swap(list, lst);

  ASSERT_EQ(lst.Size(), old_mp_size);
  ASSERT_EQ(list.Size(), old_dict_size);

  ASSERT_EQ(lst.Front(), 5);
  ASSERT_EQ(list.Front(), 15);
  ASSERT_EQ(list.Back(), 14);
}

TEST_F(ListTest, CopyConstructor) {
  List<int> lst = list;
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
  List<int> lst;
  lst.PushBack(4);
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

TEST_F(ListTest, ReverseRangeWithIteratorPreFix) {
  ASSERT_EQ(std::distance(list.Begin(), list.End()), sz) << 
                "Distanse between begin and end iterators ins't equal size";
  int iter = sz;
  for (auto it = list.End(); it != list.Begin(); --it) {
    ASSERT_EQ(*it, iter--);
  }
}

TEST_F(ListTest, ReverseRangeWithIteratorPostFix) {
  ASSERT_EQ(std::distance(list.Begin(), list.End()), sz) << 
                "Distanse between begin and end iterators ins't equal size";
  int iter = sz;
  for (auto it = list.End(); it != list.Begin(); it--) {
    ASSERT_EQ(*it, iter--);
  }
}

TEST_F(ListTest, EraseBegin) {
  int begin_value = list.Front();
  list.Erase(list.Begin());
  ASSERT_EQ(list.Size(), sz - 1);
  ASSERT_NE(list.Front(), begin_value);
}

TEST_F(ListTest, EraseMedium) {
  auto it = list.Begin();
  std::advance(it, list.Size() / 2);
  list.Erase(it);
  ASSERT_EQ(list.Size(), sz - 1);
  for (auto it = list.Begin(); it != list.End(); ++it) {
    ASSERT_NE(*it, 4);
  }
}

TEST_F(ListTest, InsertMedium) {
  auto it = list.Begin();
  std::advance(it, list.Size() / 2);
  list.Insert(it, 4);
  ASSERT_EQ(list.Size(), sz + 1);
  it = list.Begin();
  std::advance(it, list.Size() / 2);
  ASSERT_EQ(*it, 4);
  --it;
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