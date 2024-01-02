#include "table.hpp"
#include "philosopher.hpp"

#include <course/twist/test.hpp>

#include <twist/test/inject_fault.hpp>
#include <twist/test/race.hpp>
#include <twist/test/budget.hpp>

#include <vector>

using namespace dining;

void Dining(size_t seats) {
  Table table_{seats};
  std::vector<Philosopher> philosophers_;

  for (size_t seat = 0; seat < seats; ++seat) {
    philosophers_.emplace_back(table_, seat);
  }

  twist::test::Race race;

  for (size_t seat = 0; seat < seats; ++seat) {
    race.Add([&, seat]() {
      auto& plato = philosophers_.at(seat);
      while (twist::test::KeepRunning()) {
        plato.Eat();
        plato.Think();
      }
    });
  }

  race.Run();

  if (!twist::test::FaultsEnabled()) {
    // Check starvation

    std::vector<size_t> meals;
    for (auto& plato : philosophers_) {
      meals.push_back(plato.Meals());
    }

    size_t min_meals = *std::min_element(meals.begin(), meals.end());
    size_t max_meals = *std::max_element(meals.begin(), meals.end());

    std::cout << "Meals: min = " << min_meals << ", max = " << max_meals
              << std::endl;
  }
}

TEST_SUITE(DiningPhilosophers) {
  TWIST_TEST(Stress1, 3s) {
    Dining(/*seats=*/2);
  }

  TWIST_TEST(Stress2, 3s) {
    Dining(/*seats=*/3);
  }

  TWIST_TEST(Stress3, 5s) {
    Dining(/*seats=*/5);
  }

  TWIST_TEST(Stress4, 10s) {
    Dining(/*seats=*/10);
  }
}

RUN_ALL_TESTS()
