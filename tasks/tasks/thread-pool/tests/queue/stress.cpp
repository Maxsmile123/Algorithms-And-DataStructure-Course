#include <tp/queue.hpp>

#include <course/twist/test.hpp>

#include <twist/test/race.hpp>
#include <twist/test/budget.hpp>

#include <twist/ed/stdlike/atomic.hpp>

#include <atomic>

using namespace std::chrono_literals;

////////////////////////////////////////////////////////////////////////////////

namespace queue {

template <typename T>
class TestedQueue {
 public:
  TestedQueue(size_t limit): limit_(limit) {
  }

  bool TryPut(T value) {
    while (true) {
      if (size_.fetch_add(1) < limit_) {
        impl_.Put(std::move(value));
        return true;
      } else {
        size_.fetch_sub(1);
        return false;
      }
    }
  }

  std::optional<T> Take() {
    auto value = impl_.Take();
    size_.fetch_sub(1);
    return value;
  }

  void Close() {
    impl_.Close();
  }

 private:
  const size_t limit_;
  tp::UnboundedBlockingQueue<T> impl_;
  twist::ed::stdlike::atomic<size_t> size_{0};
};

void Test(size_t producers, size_t consumers) {
  TestedQueue<int> queue{33};

  std::atomic<int> consumed{0};
  std::atomic<int> produced{0};

  std::atomic<size_t> producers_left{producers};

  twist::test::Race race;

  // Producers

  for (size_t i = 0; i < producers; ++i) {
    race.Add([&, i]() {
      int value = i;
      while (twist::test::KeepRunning()) {
        if (queue.TryPut(value)) {
          produced.fetch_add(value);
          value += producers;
        }
      }

      if (producers_left.fetch_sub(1) == 1) {
        queue.Close();
      }
    });
  }

  // Consumers

  for (size_t j = 0; j < consumers; ++j) {
    race.Add([&]() {
      while (true) {
        auto value = queue.Take();
        if (value) {
          consumed.fetch_add(*value);
        } else {
          break;
        }
      }
    });
  }

  race.Run();

  ASSERT_EQ(consumed.load(), produced.load());
}

}  // namespace queue

TWIST_TEST_TEMPLATE(ProducersConsumers, queue::Test)
  ->TimeLimit(3s)
  ->Run(5, 1)
  ->Run(1, 5)
  ->Run(5, 5)
  ->Run(10, 10);

////////////////////////////////////////////////////////////////////////////////

namespace close {

void Test1() {
  tp::UnboundedBlockingQueue<int> queue;

  twist::test::Race race;

  race.Add([&]() {
    queue.Put(1);
    queue.Close();
  });

  race.Add([&]() {
    auto value = queue.Take();
    ASSERT_TRUE(value);
    ASSERT_EQ(*value, 1);
    ASSERT_FALSE(queue.Take());
  });

  race.Run();
}

void Test2() {
  tp::UnboundedBlockingQueue<int> queue;

  twist::test::Race race;

  // Producers

  race.Add([&]() {
    queue.Put(1);
  });
  race.Add([&]() {
    queue.Put(2);
  });
  race.Add([&]() {
    queue.Close();
  });

  // Consumers
  for (size_t i = 0; i < 2; ++i) {
    race.Add([&]() {
      queue.Take();
    });
  }

  race.Run();
}

}  // namespace close

TEST_SUITE(Close) {
  TWIST_TEST_REPEAT(OneConsumer, 5s) {
    close::Test1();
  }
  TWIST_TEST_REPEAT(TwoConsumers, 5s) {
    close::Test2();
  }
}

RUN_ALL_TESTS()
