#include "../map.hpp"

#include <map>

#include <iostream>

#include <chrono>
#include <gtest/gtest.h>

using namespace std;

TEST(TestGroupName, Subtest_1) {
  ASSERT_TRUE(1 == 1);
}

TEST(TestGroupName, Subtest_2) {
  ASSERT_FALSE('b' == 'b');
  cout << "continue test after failure" << endl;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}


/*
Arrange - подготовить все необходимые исходные данные для теста.

Act - запустить проверяемый метод или функцию.

Assert - сверить результат.

*/