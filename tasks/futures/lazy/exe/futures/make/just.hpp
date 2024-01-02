#pragma once

#include <exe/futures/types/future.hpp>
#include <exe/result/types/unit.hpp>

#include <exe/futures/thunks/not_implemented.hpp>

namespace exe::futures {

inline Future<Unit> auto Just() {
  return thunks::NotImplemented<Unit>{};
}

}  // namespace exe::futures
