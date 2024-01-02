#pragma once

#include <exe/futures/model/output.hpp>

namespace exe::futures {

// Represents future combinator / blocked thread / suspended stackful fiber
// or stackless coroutine

template <typename T>
struct IConsumer {
  virtual ~IConsumer() = default;

  // Invoked by producer

  void Complete(Result<T> r) noexcept {
    Consume({std::move(r), Context{}});
  }

  void Complete(Output<T> o) noexcept {
    Consume(std::move(o));
  }

  // Overriden by consumer
  virtual void Consume(Output<T>) noexcept = 0;
};

}  // namespace exe::futures
