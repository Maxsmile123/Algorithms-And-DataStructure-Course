#pragma once

#include <exe/futures/types/future.hpp>
#include <exe/result/types/unit.hpp>

namespace exe::futures {

inline Future<Unit> Just() {
  std::abort();  // Not implemented
}

}  // namespace exe::futures
