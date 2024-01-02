#pragma once

#include "table.hpp"

namespace dining {

class Philosopher {
 public:
  Philosopher(Table& table, size_t seat);

  void Eat();
  void Think();

  // Statistics
  size_t Meals() const {
    return meals_;
  }

 private:
  // Eat
  void AcquireForks();
  void EatWithForks();
  void ReleaseForks();

 private:
  Table& table_;
  size_t seat_;

  Fork& left_fork_;
  Fork& right_fork_;

  size_t meals_ = 0;
};

}  // namespace dining
