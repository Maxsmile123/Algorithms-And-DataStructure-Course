#pragma once

#include <task/join_handle.hpp>
#include <task/await.hpp>

namespace task {

inline void Join(JoinHandle /*handle*/) {
  // Not implemented
  // Await(std::move(handle));
}

}  // namespace task
