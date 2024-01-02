#pragma once

#include <hazard/mutator.hpp>

namespace hazard {

class Manager {
 public:
  static Manager* Get();

  Mutator MakeMutator();

  void Collect();
};

}  // namespace hazard
