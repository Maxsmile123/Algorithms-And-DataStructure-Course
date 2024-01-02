#pragma once

#include <asio/io_context.hpp>

namespace exe::fibers {

using Scheduler = asio::io_context;

}  // namespace exe::fibers
