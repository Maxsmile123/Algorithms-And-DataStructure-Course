#include "../../work_stealing_queue.hpp"

#include <twist/rt/layer/fault/adversary/lockfree.hpp>

#include <course/twist/test.hpp>

#include <twist/test/race.hpp>
#include <twist/test/lockfree.hpp>
#include <twist/test/budget.hpp>

#include <array>
#include <iostream>

/////////////////////////////////////////////////////////////////////

void SetupAdversary() {
#if !__has_feature(thread_sanitizer)
  twist::test::SetLockFreeAdversary();
#endif
}

/////////////////////////////////////////////////////////////////////

struct TestObject {
  size_t value;
};

class TestObjectMaker {
 public:
  TestObjectMaker() {
    PrepareNext();
  }

  ~TestObjectMaker() {
    delete next_;
  }

  TestObject* Get() {
    return next_;
  }

  void PrepareNext() {
    next_ = new TestObject{next_value_++};
  }

 private:
  size_t next_value_ = 0;
  TestObject* next_;
};

/////////////////////////////////////////////////////////////////////

template <size_t Queues, size_t Capacity>
void StressTest() {
  SetupAdversary();

  std::array<twist::test::ReportProgressFor<WorkStealingQueue<TestObject, Capacity>>, Queues> queues_;

  // Checksums
  std::atomic<size_t> produced_cs{0};
  std::atomic<size_t> consumed_cs{0};
  std::atomic<size_t> stolen_cs{0};

  twist::test::Race race;

  for (size_t i = 0; i < Queues; ++i) {
    race.Add([&, i]() {
      twist::rt::fault::GetAdversary()->EnablePark();

      size_t random = i;  // Seed

      TestObjectMaker obj_maker;

      for (size_t iter = 0; twist::test::KeepRunning(); ++iter) {
        // TryPush

        for (size_t j = 0; j < iter % 3; ++j) {
          TestObject* obj_to_push = obj_maker.Get();
          size_t obj_value = obj_to_push->value;

          if (queues_[i]->TryPush(obj_to_push)) {
            random += obj_value;
            produced_cs.fetch_add(obj_value, std::memory_order_relaxed);

            obj_maker.PrepareNext();
          }
        }

        // Grab

        TestObject* steal_buffer[5];

        if ((iter + i) % 5 == 0) {
          size_t steal_target = random % Queues;  // Pseudo-random target

          size_t stolen = queues_[steal_target]->Grab({steal_buffer, 5});

          for (size_t s = 0; s < stolen; ++s) {
            stolen_cs.fetch_add(steal_buffer[s]->value, std::memory_order_relaxed);
            delete steal_buffer[s];
          }
          continue;
        }

        // TryPop

        if (random % 2 == 0) {
          continue;
        }

        if (TestObject* obj = queues_[i]->TryPop()) {
          consumed_cs.fetch_add(obj->value, std::memory_order_relaxed);
          delete obj;
        }
      }

      // Cleanup

      while (TestObject* obj = queues_[i]->TryPop()) {
        consumed_cs += obj->value;
        delete obj;
      }
    });
  }

  race.Run();

  std::cout << "Produced: " << produced_cs.load() << std::endl;
  std::cout << "Consumed: " << consumed_cs.load() << std::endl;
  std::cout << "Stolen: "   << stolen_cs.load() << std::endl;

  ASSERT_EQ(produced_cs.load(), consumed_cs.load() + stolen_cs.load());
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(WorkStealingQueue) {
  TWIST_TEST(Stress_1, 5s) {
    StressTest</*Queues=*/2,  /*Capacity=*/5>();
  }

  TWIST_TEST(Stress_2, 5s) {
    StressTest</*Queues=*/4, /*Capacity=*/16>();
  }

  TWIST_TEST(Stress_3, 5s) {
    StressTest</*Queues=*/4, /*Capacity=*/33>();
  }

  TWIST_TEST(Stress_4, 5s) {
    StressTest</*Queues=*/4, /*Capacity=*/128>();
  }
}

RUN_ALL_TESTS()
