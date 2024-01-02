#include "../ticket_lock.hpp"

#include <course/twist/test.hpp>

#include <twist/test/inject_fault.hpp>
#include <twist/test/random.hpp>
#include <twist/test/race.hpp>
#include <twist/test/plate.hpp>
#include <twist/test/budget.hpp>

#include <twist/ed/wait/spin.hpp>

#include <cstdlib>
#include <vector>
#include <chrono>

////////////////////////////////////////////////////////////////////////////////

using namespace std::chrono_literals;

////////////////////////////////////////////////////////////////////////////////

TEST_SUITE(TicketLock) {
  void Test(size_t lockers, size_t try_lockers) {
    twist::test::Plate plate;  // Guarded by ticket_lock
    TicketLock ticket_lock;

    twist::test::Race race;

    for (size_t i = 0; i < lockers; ++i) {
      race.Add([&]() {
        while (twist::test::KeepRunning()) {
          ticket_lock.Lock();
          {
            // Critical section
            plate.Access();
          }
          ticket_lock.Unlock();
        }
      });
    }

    for (size_t j = 0; j < try_lockers; ++j) {
      race.Add([&]() {
        while (twist::test::KeepRunning()) {
          {
            // Lock
            twist::ed::SpinWait spin_wait;
            while (!ticket_lock.TryLock()) {
              spin_wait();
            }
          }
          {
            // Critical section
            plate.Access();
          }
          ticket_lock.Unlock();
        }
      });
    }

    race.Run();
  }

  TWIST_TEST(Stress1, 10s) {
    Test(2, 2);
  }

  TWIST_TEST(Stress2, 10s) {
    Test(5, 5);
  }

  TWIST_TEST(Stress3, 10s) {
    Test(10, 10);
  }
}

////////////////////////////////////////////////////////////////////////////////

namespace forks {
  using Fork = TicketLock;

  class Table {
   public:
    Table(size_t seats)
        : seats_(seats),
          plates_(seats_),
          forks_(seats_) {
    }

    Fork& LeftFork(size_t seat) {
      return forks_[seat];
    }

    Fork& RightFork(size_t seat) {
      return forks_[ToRight(seat)];
    }

    size_t ToRight(size_t seat) const {
      return (seat + 1) % seats_;
    }

    void AccessPlate(size_t seat) {
      plates_[seat].Access();
    }

   private:
    size_t seats_;
    std::vector<twist::test::Plate> plates_;
    std::vector<Fork> forks_;
  };

  class Philosopher {
   public:
    Philosopher(Table& table, size_t seat)
        : table_(table),
          seat_(seat),
          left_fork_(table_.LeftFork(seat_)),
          right_fork_(table_.RightFork(seat_)) {
    }

    void EatThenThink() {
      AcquireForks();
      Eat();
      ReleaseForks();
      Think();
    }

   private:
    void AcquireForks() {
      while (true) {
        left_fork_.Lock();
        if (right_fork_.TryLock()) {
          ASSERT_FALSE(right_fork_.TryLock());
          return;
        } else {
          left_fork_.Unlock();
        }
      }
    }

    void Eat() {
      table_.AccessPlate(seat_);
      table_.AccessPlate(table_.ToRight(seat_));
    }

    void ReleaseForks() {
      if (twist::test::Random2()) {
        ReleaseForks(left_fork_, right_fork_);
      } else {
        ReleaseForks(right_fork_, left_fork_);
      }
    };

    static void ReleaseForks(Fork& first, Fork& second) {
      first.Unlock();
      second.Unlock();
    }

    void Think() {
      twist::test::InjectFault();
    }

   private:
    Table& table_;
    size_t seat_;

    Fork& left_fork_;
    Fork& right_fork_;
  };

  void Test(size_t seats) {
    Table table{seats};

    twist::test::Race race;

    for (size_t i = 0; i < seats; ++i) {
      race.Add([&, i]() {
        Philosopher plato(table, i);
        while (twist::test::KeepRunning()) {
          plato.EatThenThink();
        }
      });
    }

    race.Run();
  }

}  // namespace philosophers

TEST_SUITE(Philosophers) {
  TWIST_TEST(Stress1, 10s) {
    forks::Test(2);
  }

  TWIST_TEST(Stress2, 10s) {
    forks::Test(5);
  }
}

////////////////////////////////////////////////////////////////////////////////

RUN_ALL_TESTS()
