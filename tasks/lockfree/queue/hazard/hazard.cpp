#include <hazard/manager.hpp>

#include <twist/ed/local/ptr.hpp>

namespace hazard {

Manager* Manager::Get() {
  static Manager instance;
  return &instance;
}

Mutator Manager::MakeMutator() {
  std::abort();  // Not implemented
}

void Manager::Collect() {
  // Not implemented
}

}  // namespace hazard
