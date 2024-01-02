#pragma once

#include <chrono>

namespace exe::fibers {

using Millis = std::chrono::milliseconds;

void SleepFor(Millis);

}  // namespace exe::fibers
