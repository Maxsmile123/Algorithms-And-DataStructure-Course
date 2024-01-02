#pragma once

#include <wheels/test/current.hpp>

inline bool KeepRunning() {
  return wheels::test::TestTimeLeft() > std::chrono::milliseconds(500);
}
