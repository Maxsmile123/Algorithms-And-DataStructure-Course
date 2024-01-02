#pragma once

#include <exe/futures/types/future.hpp>
#include <exe/futures/types/eager.hpp>

#include <exe/result/types/result.hpp>

#include <tuple>

namespace exe::futures {

template <typename T>
class Promise {
 public:
  void Set(Result<T>) && {
    // Not implemented
  }

  void SetValue(T) && {
    // Not implemented
  }

  void SetError(Error) && {
    // Not implemented
  }

 private:
  // ???
};

template <typename T>
std::tuple<EagerFuture<T>, Promise<T>> Contract() {
  std::abort();  // Not implemented
}

}  // namespace exe::futures
