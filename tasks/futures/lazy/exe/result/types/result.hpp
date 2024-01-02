#pragma once

#include <tl/expected.hpp>

#include <exe/result/types/error.hpp>

namespace exe {

// https://github.com/TartanLlama/expected

template <typename T>
using Result = tl::expected<T, Error>;

}  // namespace exe
