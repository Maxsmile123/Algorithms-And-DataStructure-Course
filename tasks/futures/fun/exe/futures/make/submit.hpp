#pragma once

#include <exe/futures/types/future.hpp>
#include <exe/executors/executor.hpp>

#include <exe/result/traits/value_of.hpp>

#include <type_traits>

namespace exe::futures {

namespace traits {

template <typename F>
using SubmitT = result::traits::ValueOf<std::invoke_result_t<F>>;

}  // namespace traits

template <typename F>
Future<traits::SubmitT<F>> Submit(executors::IExecutor& /*exe*/, F /*fun*/) {
  std::abort();  // Not implemented
}

}  // namespace exe::futures
