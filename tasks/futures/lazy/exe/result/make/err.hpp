#pragma once

#include <exe/result/types/result.hpp>

namespace exe::result {

/*
 *
 * Usage:
 *
 * futures::Just() | futures::AndThen([](Unit) -> Result<int> {
 *   return result::Err(Timeout());
 * });
 *
 */

inline auto Err(Error error) {
  return tl::unexpected(error);
}

}  // namespace exe::result
