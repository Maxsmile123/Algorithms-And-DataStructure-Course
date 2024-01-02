#pragma once

#include <exe/futures/model/consumer.hpp>

namespace exe::futures {

// https://wiki.haskell.org/Thunk
// Represents arbitrary (asynchronous) computation

template <typename T>
concept Thunk = requires(T thunk, IConsumer<typename T::ValueType>* consumer) {
  typename T::ValueType;

  // Start evaluation
  thunk.Start(consumer);
};

}  // namespace exe::futures
