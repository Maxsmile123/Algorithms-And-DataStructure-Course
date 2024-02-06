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
      assert(mp.Size() == sz);
    }

  Map<int, int> mp;
  const size_t sz = 7;
};


TEST(EmptyMapTest, DefaultConstructor) {
  Map<int, int> map;
  ASSERT_TRUE(map.IsEmpty()) << "Default Map isn't empty!";
}

TEST(EmptyMapTest, InsertRoot) {
  Map<int, int> map;
  map.Insert({1, 5});
  ASSERT_EQ(map.Size(), 1);

  auto vals = map.Values(true);

  ASSERT_EQ(vals.size(), 1);
  ASSERT_EQ(vals[0].first, 1);
  ASSERT_EQ(vals[0].second, 5);
}

TEST(EmptyMapTest, InsertRootLeftRight) {
  Map<int, int> map;
  map.Insert({1, 1});
  map.Insert({3, 2});
  map.Insert({0, 0});

  ASSERT_EQ(map.Size(), 3);

  auto vals = map.Values(true);
  ASSERT_EQ(vals.size(), 3);

  for (size_t i = 0; i < vals.size(); ++i) {
    ASSERT_EQ(vals[i].second, i) <<
                    fmt::format("Values isn't equal on {} index", i);
  }
}

TEST(EmptyMapTest, InsertIncreaseSeq) {
  Map<int, int> map;
  map.Insert({
        {1, 0},
        {3, 1},
        {5, 2},
        {10, 3},
        {90, 4}
      });
  ASSERT_EQ(map.Size(), 5);
  
  auto vals = map.Values(true);
  ASSERT_EQ(vals.size(), 5);

  for (size_t i = 0; i < vals.size(); ++i) {
    ASSERT_EQ(vals[i].second, i) <<
                    fmt::format("Values isn't equal on {} index", i);
  }
}

TEST(EmptyMapTest, SimpleSwap) {
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

TEST(EmptyMapTest, StdSwap) {
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

TEST(EmptyMapTest, EraseOnlyRoot) {
  Map<int, int> mp;
  mp.Insert({1, 2});
  mp.Erase(1);
  ASSERT_EQ(mp.Size(), 0);

  auto vals = mp.Values(true);
  ASSERT_TRUE(vals.empty());
}

TEST(EmptyMapTest, StringAsKey) {
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

TEST_F(MapTest, GetValueUsingOperator) {
  ASSERT_EQ(mp[5], 90);
  ASSERT_EQ(mp[-10], 5);
  ASSERT_EQ(mp[1], 5);
  ASSERT_EQ(mp[0], 4);
}

TEST_F(MapTest, OverwritingWithOperator) {
  mp[5] = 5;
  mp[-10] = 10;
  ASSERT_EQ(mp[5], 5);
  ASSERT_EQ(mp[-10], 10);
}

TEST_F(MapTest, CreateIfNotExist) {
  mp[-1];
  ASSERT_EQ(mp[-1], 0); // default for type value
  ASSERT_EQ(mp.Size(), sz + 1);
}

TEST_F(MapTest, GetIncreaseSortedValues) {
  auto values = mp.Values(true);

  for (size_t i = 1; i < values.size(); ++i) {
    ASSERT_LT(values[i - 1].first, values[i].first) <<
                    fmt::format("Doesn't increase starting with {} index", i);
  }
}

TEST_F(MapTest, GetDecreaseSortedValues) {
  auto values = mp.Values(false);

  for (size_t i = 1; i < values.size(); ++i) {
    ASSERT_GT(values[i - 1].first, values[i].first) <<
                    fmt::format("Doesn't decrease starting with {} index", i);
  }
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

TEST_F(MapTest, EraseLeaf) {
  mp.Erase(0);
  ASSERT_EQ(mp.Size(), sz - 1);

  auto vals = mp.Values(true);
  ASSERT_EQ(vals.size(), sz - 1);

  for (size_t i = 1; i < vals.size(); ++i) {
    ASSERT_NE(vals[i - 1].first, 0);
    ASSERT_LT(vals[i - 1].first, vals[i].first) <<
                    fmt::format("Doesn't increase starting with {} index", i);
  }
}

TEST_F(MapTest, EraseNodeWithRightSon) {
  mp.Erase(-10);
  ASSERT_EQ(mp.Size(), sz - 1);

  auto vals = mp.Values(true);
  ASSERT_EQ(vals.size(), sz - 1);

  for (size_t i = 1; i < vals.size(); ++i) {
    ASSERT_NE(vals[i - 1].first, -10);
    ASSERT_LT(vals[i - 1].first, vals[i].first) <<
                    fmt::format("Doesn't increase starting with {} index", i);
  }
}

TEST_F(MapTest, EraseNodeWithLeftSon) {
  mp.Erase(3);
  ASSERT_EQ(mp.Size(), sz - 1);

  auto vals = mp.Values(true);
  ASSERT_EQ(vals.size(), sz - 1);

  for (size_t i = 1; i < vals.size(); ++i) {
    ASSERT_NE(vals[i - 1].first, 3);
    ASSERT_LT(vals[i - 1].first, vals[i].first) <<
                    fmt::format("Doesn't increase starting with {} index", i);
  }
}

TEST_F(MapTest, EraseNodeWithTwoSons) {
  mp.Erase(1);
  ASSERT_EQ(mp.Size(), sz - 1);

  auto vals = mp.Values(true);
  ASSERT_EQ(vals.size(), sz - 1);

  for (size_t i = 1; i < vals.size(); ++i) {
    ASSERT_NE(vals[i - 1].first, 1);
    ASSERT_LT(vals[i - 1].first, vals[i].first) <<
                    fmt::format("Doesn't increase starting with {} index", i);
  }
}

TEST_F(MapTest, EraseSeveralValues) {
  mp.Erase(3);
  mp.Erase(-10);
  mp.Erase(0);
  ASSERT_EQ(mp.Size(), sz - 3);

  auto vals = mp.Values(true);
  ASSERT_EQ(vals.size(), sz - 3);

  for (size_t i = 1; i < vals.size(); ++i) {
    ASSERT_NE(vals[i - 1].first, 0);
    ASSERT_NE(vals[i - 1].first, 3);
    ASSERT_NE(vals[i - 1].first, -10);
    ASSERT_LT(vals[i - 1].first, vals[i].first) <<
                    fmt::format("Doesn't increase starting with {} index", i);
  }
}

TEST_F(MapTest, EraseNotExistingValue) {
  EXPECT_THROW({
    mp.Erase(-100);
  }, std::runtime_error);
}

TEST_F(MapTest, CustomComparator) {
  struct Point {
    int x;
    int y;
    bool operator==(const Point& b) {
      return (this->x == b.x) && (this->y == b.y);
    }
  };

  struct PointComparator {
    constexpr bool operator()(const Point& a, const Point& b) const {
        auto dist1 = sqrt(pow(a.x, 2) + pow(a.y, 2));
        auto dist2 = sqrt(pow(b.x, 2) + pow(b.y, 2));
        return dist1 < dist2;
    }
  };

  Map<Point, int, PointComparator> points;
  points.Insert({
    {{0, 0}, 21},
    {{4, 5}, 22},
    {{0, 10}, 24},
  });

  std::map<Point, int, PointComparator> std_points{
      {{0, 0}, 21},
      {{4, 5}, 22},
      {{0, 10}, 24},
  };

  auto values = points.Values(true);
  auto it = values.begin();
  for (const auto& val: std_points) {
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