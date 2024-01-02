#pragma once

#include <twist/ed/stdlike/mutex.hpp>
#include <twist/test/plate.hpp>

#include <vector>

namespace dining {

using Fork = twist::ed::stdlike::mutex;

using Plate = twist::test::Plate;

class Table {
 public:
  explicit Table(size_t num_seats)
      : num_seats_(num_seats),
        plates_(num_seats_),
        forks_(num_seats_) {
  }

  Fork& LeftFork(size_t seat) {
    return forks_[seat];
  }

  Fork& RightFork(size_t seat) {
    return forks_[ToRight(seat)];
  }

  size_t ToRight(size_t seat) const {
    return (seat + 1) % num_seats_;
  }

  void AccessPlate(size_t seat) {
    plates_[seat].Access();
  }

 private:
  const size_t num_seats_;
  std::vector<Plate> plates_;
  std::vector<Fork> forks_;
};

}  // namespace dining
