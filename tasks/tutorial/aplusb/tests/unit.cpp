#include <gtest/gtest.h>

#include "../aplusb.hpp"


TEST(Sum, Simple) {
  ASSERT_EQ(Sum(1, 1), 2);
}

TEST(Sum, Zero) {
  ASSERT_EQ(Sum(0, 0), 0);
}

TEST(Sum, NegativeAndPositive) {
  ASSERT_EQ(Sum(-1, 1), 0);
}

TEST(Sum, TwoNegative) {
  ASSERT_EQ(Sum(-1, -1), -2);
}

TEST(Sum, Overflow) {
  ASSERT_EQ(Sum(INT_MAX, 1), INT_MAX);
}

TEST(Sum, Fun) {
  ASSERT_EQ(Sum(INT_MAX, INT_MIN + 1), 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}