#pragma once

#include <exe/futures/types/future.hpp>
#include <exe/executors/executor.hpp>

#include <exe/result/traits/value_of.hpp>

#include <exe/futures/thunks/not_implemented.hpp>

#include <type_traits>

namespace exe::futures {

namespace traits {

template <typename F>
using SubmitT = result::traits::ValueOf<std::invoke_result_t<F>>;

}  // namespace traits

template <typename F>
Future<traits::SubmitT<F>> auto Submit(executors::IExecutor& /*exe*/,
                                       F /*fun*/) {
  return thunks::NotImplemented<traits::SubmitT<F>>{};
}

}  // namespace exe::futures
