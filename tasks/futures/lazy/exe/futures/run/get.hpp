#pragma once

#include <exe/futures/syntax/pipe.hpp>

#include <exe/futures/traits/value_of.hpp>

#include <exe/result/types/result.hpp>

namespace exe::futures {

namespace pipe {

struct [[nodiscard]] Get {
  template <SomeFuture InputFuture>
  Result<traits::ValueOf<InputFuture>> Pipe(InputFuture) {
    std::abort();  // Not implemented
  }
};

}  // namespace pipe

inline auto Get() {
  return pipe::Get{};
}

}  // namespace exe::futures
