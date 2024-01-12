#include <chrono>
#include <future>
#include <map>
#include <iostream>
#include <string>
#include <thread>

#include <fmt/core.h>
#include <gtest/gtest.h>

#include "../map.hpp"

class MapTest: public testing::Test {
  protected:
    void SetUp() override {
      mp.Insert({
        {1, 5},
        {3, 10},
        {5, 90},
        {10, -10},
        {90, 0},
        {-10, 5},
        {0, 4}
      });
    }

  Map<int, int> mp;
  const size_t sz = 7;
};


TEST_F(MapTest, DefaultConstructor) {
  Map<int, int> map;
  ASSERT_TRUE(map.IsEmpty()) << "Default Map isn't empty!";
}

TEST_F(MapTest, InsertRoot) {
  Map<int, int> map;
  map.Insert({1, 5});
  ASSERT_TRUE(map.Size() == 1);

  auto vals = map.Values(true);

  ASSERT_TRUE(vals.size() == 1);
  ASSERT_EQ(vals[0].first, 1);
  ASSERT_EQ(vals[0].second, 5);
}

TEST_F(MapTest, GetValueUsingOperator) {
  ASSERT_EQ(mp[5], 90);
  ASSERT_EQ(mp[-10], 5);
  ASSERT_EQ(mp[1], 5);
  ASSERT_EQ(mp[0], 4);
}

TEST_F(MapTest, OverwritingWithOperator) {
  mp[5] = 5;
  mp[-10] = 10
  ASSERT_EQ(mp[5], 5);
  ASSERT_EQ(mp[-10], 10);
}

TEST_F(MapTest, CreateIfNotExist) {
  Map<int, int> map;
  map[1];
  ASSERT_EQ(map[1], 0); // default for type value
  ASSERT_EQ(map.Size(), 1);
}

TEST_F(MapTest, GetIncreaseSortedValues) {
  auto values = mp.Values(true);

  for (size_t i = 1; i < values.size(); ++i) {
    ASSERT_LT(values[i - 1].second, values[i].second) <<
                    fmt::format("Doesn't increase starting with {} index", i);
  }
}

TEST_F(MapTest, GetDecreaseSortedValues) {
  auto values = mp.Values(false);

  for (size_t i = 1; i < values.size(); ++i) {
    ASSERT_GT(values[i - 1].second, values[i].second) <<
                    fmt::format("Doesn't decrease starting with {} index", i);
  }
}

TEST_F(MapTest, InsertRootLeftRight) {
  Map<int, int> map;
  map.Insert({1, 1});
  map.Insert({3, 2});
  map.Insert({0, 0});

  ASSERT_TRUE(map.Size() == 3);

  auto vals = map.Values(true);
  ASSERT_TRUE(vals.size() == 3);

  for (size_t i = 0; i < vals.size(); ++i) {
    ASSERT_EQ(vals[i].second, i) <<
                    fmt::format("Values isn't equal on {} index", i);
  }
}

TEST_F(MapTest, MoveInsert) {
  Map<int, int> map;
  std::pair<const int, int> a(1, 5);
  map.Insert(std::move(a));
  ASSERT_TRUE(map.Size() == 1);
  auto vals = map.Values(true);
  ASSERT_TRUE(vals.size() == 1);
  ASSERT_EQ(vals[0].first, 1);
  ASSERT_EQ(vals[0].second, 5);
}

TEST_F(MapTest, InsertIncreaseSeq) {
  ASSERT_TRUE(mp.Size() == sz);
  
  auto vals = mp.Values(true);
  ASSERT_TRUE(vals.size() == sz);

  for (size_t i = 0; i < vals.size(); ++i) {
    ASSERT_EQ(vals[i].second, i) <<
                    fmt::format("Values isn't equal on {} index", i);
  }
}

TEST_F(MapTest, SimpleSwap) {
  Map<int, int> map;
  map[1] = 5;

  Map<int, int> dict;
  dict[1] = 15;
  dict[2] = 14;

  size_t old_mp_size = map.Size();
  size_t old_dict_size = dict.Size();

  map.Swap(dict);

  ASSERT_EQ(dict.Size(), old_mp_size);
  ASSERT_EQ(map.Size(), old_dict_size);

  ASSERT_EQ(dict[1], 5);
  ASSERT_EQ(map[1], 15);
  ASSERT_EQ(map[2], 14);
}

TEST_F(MapTest, StdSwap) {
  Map<int, int> map;
  map[1] = 5;

  Map<int, int> dict;
  dict[1] = 15;
  dict[2] = 14;

  size_t old_mp_size = map.Size();
  size_t old_dict_size = dict.Size();
  
  std::swap(map, dict);

  ASSERT_EQ(dict.Size(), old_mp_size);
  ASSERT_EQ(map.Size(), old_dict_size);

  ASSERT_EQ(dict[1], 5);
  ASSERT_EQ(map[1], 15);
  ASSERT_EQ(map[2], 14);
}

TEST_F(MapTest, SwapYourself) {
  Map<int, int> map;
  auto old_adr = &map;
  mp.Swap(map);
  ASSERT_EQ(&map, old_adr);
}

TEST_F(MapTest, CopyConstructor) {
  Map<int, int> mp_copy = mp;

  ASSERT_EQ(mp.Size(), mp_copy.Size());

  auto mp_values = mp.Values(true);
  auto mp_copy_values = mp_copy.Values(true);

  for (size_t i = 0; i < mp_values.size(); ++i) {
    ASSERT_EQ(mp_values[i], mp_copy_values[i]) <<
                    fmt::format("Values isn't equal on {} index\n", i);
  }

  ASSERT_NE(&mp, &mp_copy) << "It's the same object! There must be a copy!\n";
}

TEST_F(MapTest, CopyEmptyMap) {
  Map<int, int> map;
  Map<int, int> mp_copy = map;

  ASSERT_EQ(map.IsEmpty(), mp_copy.IsEmpty());
  ASSERT_NE(&map, &mp_copy) << "It's the same object! There must be a copy!\n";
}


TEST_F(MapTest, CopyOperator) {
  Map<int, int> mp_copy;

  mp_copy = mp;

  ASSERT_EQ(mp.Size(), mp_copy.Size());

  auto mp_values = mp.Values(true);
  auto mp_copy_values = mp_copy.Values(true);

  for (size_t i = 0; i < mp_values.size(); ++i) {
    ASSERT_EQ(mp_values[i], mp_copy_values[i]) <<
                    fmt::format("Values isn't equal on {} index", i);
  }

  ASSERT_NE(&mp, &mp_copy) << "It's the same object! There must be a copy!";
}


TEST_F(MapTest, SelfAssignment) {
  Map<int, int> mp;
  std::thread thread([&](){
    mp = mp;
  });

  auto future = std::async(std::launch::async, &std::thread::join, &thread);
  ASSERT_EQ(
    future.wait_for(std::chrono::seconds(1)),
    std::future_status::timeout
  ) << "There is infinity loop!\n";
}

TEST_F(MapTest, Clear) {
  mp.Clear();
  ASSERT_TRUE(mp.IsEmpty());
  ASSERT_EQ(mp.Size(), 0);
}

TEST_F(MapTest, FindExistValue) {
  ASSERT_TRUE(mp.Find(3));
}

TEST_F(MapTest, FindNotExistValue) {
  ASSERT_FALSE(mp.Find(-11));
}

TEST_F(MapTest, EraseOneValue) {
  mp.Erase(3);
  ASSERT_EQ(mp.Size(), sz - 1);

  auto vals = mp.Values(true);
  ASSERT_TRUE(vals.size() == sz - 1);

  for (size_t i = 1; i < vals.size(); ++i) {
    ASSERT_LT(vals[i - 1].second, vals[i].second) <<
                    fmt::format("Doesn't increase starting with {} index", i);
  }
}

TEST_F(MapTest, EraseSeveralValues) {
  mp.Erase(3);
  mp.Erase(-10);
  mp.Erase(0);
  ASSERT_EQ(mp.Size(), sz - 3);

  auto vals = mp.Values(true);
  ASSERT_TRUE(vals.size() == sz - 3);

  for (size_t i = 1; i < vals.size(); ++i) {
    ASSERT_LT(vals[i - 1].second, vals[i].second) <<
                    fmt::format("Doesn't increase starting with {} index", i);
  }
}

TEST_F(MapTest, EraseNotExistingValue) {
  EXPECT_THROW({
    mp.Erase(-100);
  }, std::runtime_error);
}

TEST_F(MapTest, StringAsKey) {
  Map<std::string, int> ages;
  ages.Insert({
    {"Maxim", 21},
    {"Danya", 22},
    {"Veronika", 24},
    {"Anna", 19}
  });
  std::map<std::string, int> std_ages{
    {"Maxim", 21},
    {"Danya", 22},
    {"Veronika", 24},
    {"Anna", 19}
  };
  auto values = ages.Values(true);
  auto it = values.begin();
  for (const auto& val: std_ages) {
    ASSERT_EQ(it->second, val.second) <<
                fmt::format("Values isn't equal on {} index", 
                    std::distance(values.begin(), it)
                );
    ++it;
  }

}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}