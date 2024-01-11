#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>

#include <fmt/core.h>
#include <gtest/gtest.h>

#include "../map.hpp"

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}