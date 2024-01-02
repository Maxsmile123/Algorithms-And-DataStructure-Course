#include <exe/executors/thread_pool.hpp>

#include <exe/fibers/core/api.hpp>
#include <exe/fibers/sync/channel.hpp>
#include <exe/fibers/sync/select.hpp>

#include <wheels/test/framework.hpp>
#include <twist/test/budget.hpp>

#include <course/twist/test.hpp>
#include <twist/fault/adversary/adversary.hpp>

#include <map>
#include <random>

//////////////////////////////////////////////////////////////////////

using namespace exe;

class SelectTester {
  class StartLatch {
   public:
    void Release() {
      started_.store(true);
    }

    void Await() {
      while (!started_.load()) {
        fibers::self::Yield();
      }
    }

   private:
    std::atomic<bool> started_{false};
  };

 public:
  explicit SelectTester(size_t threads) : pool_(threads) {
  }

  void AddChannel(size_t capacity) {
    channels_.emplace_back(capacity);
  }

  void Produce(size_t i) {
    producers_[i].fetch_add(1);

    fibers::Go(pool_, [this, i]() {
      Producer(i);
    });
  }

  void Receive(size_t i) {
    fibers::Go(pool_, [this, i]() {
      ReceiveConsumer(i);
    });
  }

  void Select(size_t i, size_t j) {
    fibers::Go(pool_, [this, i, j]() {
      SelectConsumer(i, j);
    });
  }

  void TrySelect(size_t i, size_t j) {
    fibers::Go(pool_, [this, i, j]() {
      TrySelectConsumer(i, j);
    });
  }

  void RunTest() {
    twist::fault::GetAdversary()->Reset();

    // Release all fibers
    start_latch_.Release();

    pool_.WaitIdle();

    // Print report
    std::cout << "Sends #: " << sends_.load() << std::endl;
    std::cout << "Receives #: " << receives_.load() << std::endl;

    ASSERT_EQ(sends_.load(), receives_.load());

    std::cout << "Produced: " << total_produced_.load() << std::endl;
    std::cout << "Consumed: " << total_consumed_.load() << std::endl;

    ASSERT_EQ(total_produced_.load(), total_consumed_.load());

    //twist::fault::GetAdversary()->PrintReport();

    pool_.Stop();
  }

 private:
  void SelectConsumer(size_t i, size_t j) {
    start_latch_.Await();

    auto xs = channels_[i];
    auto ys = channels_[j];

    bool xs_done = false;
    bool ys_done = false;

    size_t iter = 0;

    while (true) {
      if (++iter % 7 == 0) {
        fibers::self::Yield();
      }

      auto selected_value = fibers::Select(xs, ys);

      switch (selected_value.index()) {
        case 0: {
          int64_t x = std::get<0>(selected_value);
          if (x == -1) {
            xs_done = true;
            xs.Send(-1);
          } else {
            total_consumed_.fetch_add(x, std::memory_order_relaxed);
            receives_.fetch_add(1, std::memory_order_relaxed);
          }
          break;
        }
        case 1: {
          int64_t y = std::get<1>(selected_value);
          if (y == -1) {
            ys_done = true;
            ys.Send(-1);
          } else {
            total_consumed_.fetch_add(y, std::memory_order_relaxed);
            receives_.fetch_add(1, std::memory_order_relaxed);
          }
          break;
        }
      }

      if (xs_done || ys_done) {
        break;
      }
    }

    if (xs_done) {
      ReceiveConsumer(j);
    } else {
      ReceiveConsumer(i);
    }
  }

  void TrySelectConsumer(size_t i, size_t j) {
    start_latch_.Await();

    auto xs = channels_[i];
    auto ys = channels_[j];

    bool xs_done = false;
    bool ys_done = false;

    size_t iter = 0;

    while (true) {
      if (++iter % 7 == 0) {
        fibers::self::Yield();
      }

      auto selected_value = fibers::TrySelect(xs, ys);

      switch (selected_value.index()) {
        case 0: {
          int64_t x = std::get<0>(selected_value);
          if (x == -1) {
            xs_done = true;
            xs.Send(-1);
          } else {
            total_consumed_.fetch_add(x, std::memory_order_relaxed);
            receives_.fetch_add(1, std::memory_order_relaxed);
          }
          break;
        }
        case 1: {
          int64_t y = std::get<1>(selected_value);
          if (y == -1) {
            ys_done = true;
            ys.Send(-1);
          } else {
            total_consumed_.fetch_add(y, std::memory_order_relaxed);
            receives_.fetch_add(1, std::memory_order_relaxed);
          }
          break;
        }
        default: {
          fibers::self::Yield();
          break;
        }
      }

      if (xs_done || ys_done) {
        break;
      }
    }

    if (xs_done) {
      ReceiveConsumer(j);
    } else {
      ReceiveConsumer(i);
    }
  }

  void ReceiveConsumer(size_t i) {
    start_latch_.Await();

    auto xs = channels_[i];

    size_t iter = 0;

    while (true) {
      if (++iter % 5 == 0) {
        fibers::self::Yield();
      }

      auto value = xs.Receive();
      if (value == -1) {
        xs.Send(-1);
        break;
      }

      total_consumed_.fetch_add(value, std::memory_order_relaxed);
      receives_.fetch_add(1, std::memory_order_relaxed);
    }
  }

  void Producer(size_t i) {
    start_latch_.Await();

    std::mt19937 twister{/*seed=*/(uint32_t)i};

    auto channel = channels_[i];

    while (twist::test::KeepRunning()) {
      int64_t value = twister();

      channel.Send(value);

      total_produced_.fetch_add(value, std::memory_order_relaxed);
      sends_.fetch_add(1, std::memory_order_relaxed);
    }

    if (producers_[i].fetch_sub(1) == 1) {
      // Last producer
      channel.Send(-1);  // Send poison pill
    }
  }

 private:
  executors::ThreadPool pool_;

  std::vector<fibers::Channel<int64_t>> channels_;

  std::map<size_t, std::atomic<size_t>> producers_;

  StartLatch start_latch_;

  std::atomic<int64_t> sends_{0};
  std::atomic<int64_t> receives_{0};

  std::atomic<int64_t> total_produced_{0};
  std::atomic<int64_t> total_consumed_{0};
};

//////////////////////////////////////////////////////////////////////

void AtomicityStressTest() {
  executors::ThreadPool pool_{4};

  fibers::Go(pool_, []() {
    size_t iter = 0;

    while (twist::test::KeepRunning()) {
      ++iter;

      fibers::Channel<int> xs{1};
      fibers::Channel<int> ys{1};

      xs.Send(1);

      fibers::Go([xs, ys]() mutable {
        ys.Send(2);
        xs.Receive();
      });

      if (iter % 2 == 0) {
        auto selected_value = fibers::TrySelect(xs, ys);
        ASSERT_TRUE(selected_value.index() != 2);
      } else {
        auto selected_value = fibers::TrySelect(ys, xs);
        ASSERT_TRUE(selected_value.index() != 2);
      }

      // Wake up
      xs.Send(4);
    }
  });

  pool_.WaitIdle();
  pool_.Stop();
}

TEST_SUITE(TrySelect) {
  TWIST_TEST(ConcurrentSelects, wheels::test::TestOptions().TimeLimit(5s)) {
    SelectTester tester{/*threads=*/4};

    tester.AddChannel(7);
    tester.AddChannel(8);

    tester.Produce(0);
    tester.Produce(1);

    tester.TrySelect(0, 1);
    tester.TrySelect(0, 1);

    tester.RunTest();
  }

  TWIST_TEST(Deadlock, wheels::test::TestOptions().TimeLimit(5s)) {
    SelectTester tester{/*threads=*/4};

    tester.AddChannel(17);
    tester.AddChannel(17);

    tester.Produce(0);
    tester.Produce(1);

    tester.TrySelect(0, 1);
    tester.TrySelect(1, 0);

    tester.RunTest();
  }

  TWIST_TEST(MixTrySelectsAndReceives, wheels::test::TestOptions().TimeLimit(5s)) {
    SelectTester tester{/*threads=*/4};

    tester.AddChannel(11);
    tester.AddChannel(9);

    tester.Produce(0);
    tester.Produce(1);
    tester.Produce(1);

    tester.TrySelect(0, 1);
    tester.TrySelect(1, 0);
    tester.Receive(0);
    tester.Receive(1);

    tester.RunTest();
  }

  TWIST_TEST(OverlappedTrySelects, wheels::test::TestOptions().TimeLimit(5s)) {
    SelectTester tester{/*threads=*/4};

    tester.AddChannel(11);
    tester.AddChannel(12);
    tester.AddChannel(13);

    tester.Produce(0);
    tester.Produce(1);
    tester.Produce(2);

    tester.TrySelect(0, 1);
    tester.TrySelect(1, 2);
    tester.TrySelect(2, 0);

    tester.RunTest();
  }

  TWIST_TEST(Hard, wheels::test::TestOptions().TimeLimit(5s)) {
    SelectTester tester{/*threads=*/4};

    tester.AddChannel(7);
    tester.AddChannel(9);
    tester.AddChannel(8);

    tester.Produce(0);
    tester.Produce(0);
    tester.Produce(1);
    tester.Produce(1);
    tester.Produce(2);
    tester.Produce(2);
    tester.Produce(2);

    tester.TrySelect(0, 1);
    tester.TrySelect(1, 0);
    tester.TrySelect(1, 2);
    tester.TrySelect(2, 1);
    tester.TrySelect(2, 0);
    tester.TrySelect(0, 2);

    tester.Receive(0);
    tester.Receive(1);
    tester.Receive(1);
    tester.Receive(2);

    tester.RunTest();
  }

  TWIST_TEST(Atomicity, wheels::test::TestOptions().TimeLimit(5s)) {
    AtomicityStressTest();
  }
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(Select) {
  TWIST_TEST(ConcurrentSelects, wheels::test::TestOptions().TimeLimit(5s)) {
    SelectTester tester{/*threads=*/4};

    tester.AddChannel(7);
    tester.AddChannel(8);

    tester.Produce(0);
    tester.Produce(1);

    tester.Select(0, 1);
    tester.Select(0, 1);

    tester.RunTest();
  }

  TWIST_TEST(Deadlock, wheels::test::TestOptions().TimeLimit(5s)) {
    SelectTester tester{/*threads=*/4};

    tester.AddChannel(17);
    tester.AddChannel(17);

    tester.Produce(0);
    tester.Produce(1);

    tester.Select(0, 1);
    tester.Select(1, 0);

    tester.RunTest();
  }

  TWIST_TEST(MixSelectsAndReceives, wheels::test::TestOptions().TimeLimit(5s)) {
    SelectTester tester{/*threads=*/4};

    tester.AddChannel(11);
    tester.AddChannel(9);

    tester.Produce(0);
    tester.Produce(1);

    tester.Select(0, 1);
    tester.Select(1, 0);
    tester.Receive(0);
    tester.Receive(1);

    tester.RunTest();
  }

  TWIST_TEST(OverlappedSelects, wheels::test::TestOptions().TimeLimit(5s)) {
    SelectTester tester{/*threads=*/4};

    tester.AddChannel(11);
    tester.AddChannel(12);
    tester.AddChannel(13);

    tester.Produce(0);
    tester.Produce(1);
    tester.Produce(2);

    tester.Select(0, 1);
    tester.Select(1, 2);
    tester.Select(2, 0);

    tester.RunTest();
  }

  TWIST_TEST(Hard, wheels::test::TestOptions().TimeLimit(5s)) {
    SelectTester tester{/*threads=*/4};

    tester.AddChannel(7);
    tester.AddChannel(9);
    tester.AddChannel(8);

    tester.Produce(0);
    tester.Produce(0);
    tester.Produce(1);
    tester.Produce(1);
    tester.Produce(2);
    tester.Produce(2);
    tester.Produce(2);

    tester.Select(0, 1);
    tester.Select(1, 0);
    tester.Select(1, 2);
    tester.Select(2, 1);
    tester.Select(2, 0);
    tester.Select(0, 2);

    tester.TrySelect(0, 1);
    tester.TrySelect(2, 1);

    tester.Receive(0);
    tester.Receive(1);
    tester.Receive(1);
    tester.Receive(2);

    tester.RunTest();
  }
}

//////////////////////////////////////////////////////////////////////

RUN_ALL_TESTS()
