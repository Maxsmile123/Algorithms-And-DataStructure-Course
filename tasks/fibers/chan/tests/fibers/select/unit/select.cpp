#include <exe/executors/thread_pool.hpp>
#include <exe/executors/manual.hpp>

#include <exe/fibers/core/api.hpp>
#include <exe/fibers/sync/channel.hpp>
#include <exe/fibers/sync/select.hpp>

#include <wheels/test/framework.hpp>
#include <wheels/test/cpu_time.hpp>

#include <thread>

using namespace exe;

//////////////////////////////////////////////////////////////////////

void RunScheduler(size_t threads, std::function<void()> routine) {
  executors::ThreadPool scheduler{threads};

  fibers::Go(scheduler, routine);

  scheduler.WaitIdle();
  scheduler.Stop();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(TrySelect) {
  SIMPLE_TEST(NoValue) {
    RunScheduler(1, []() {
      fibers::Channel<int> ints{1};
      fibers::Channel<std::string> strs{1};

      auto selected_value = fibers::TrySelect(ints, strs);

      // No value
      ASSERT_EQ(selected_value.index(), 2);
    });
  }

  SIMPLE_TEST(Values1) {
    bool done = false;

    RunScheduler(4, [&done]() {
      fibers::Channel<int> ints{5};
      fibers::Channel<std::string> strs{5};

      {
        ints.Send(17);

        auto selected_value = fibers::TrySelect(ints, strs);

        ASSERT_EQ(selected_value.index(), 0);
        ASSERT_EQ(std::get<0>(selected_value), 17);
      }

      {
        strs.Send("Hi");

        auto selected_value = fibers::TrySelect(ints, strs);

        ASSERT_EQ(selected_value.index(), 1);
        ASSERT_EQ(std::get<1>(selected_value), "Hi");
      }

      {
        auto selected_value = fibers::TrySelect(ints, strs);

        ASSERT_EQ(selected_value.index(), 2);
      }

      done = true;
    });

    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(Values2) {
    bool done = false;

    RunScheduler(4, [&done]() {
      fibers::Channel<int> ints{5};
      fibers::Channel<std::string> strs{5};

      ints.Send(44);
      strs.Send("Test");

      bool int_received = false;
      bool str_received = false;

      for (size_t i = 0; i < 2; ++i) {
        auto selected_value = fibers::TrySelect(strs, ints);

        if (selected_value.index() == 0) {
          ASSERT_EQ(std::get<0>(selected_value), "Test");
          str_received = true;
        } else if (selected_value.index() == 1) {
          ASSERT_EQ(std::get<1>(selected_value), 44);
          int_received = true;
        } else {
          FAIL_TEST("Impossible");
        }
      }

      ASSERT_TRUE(int_received && str_received);

      done = true;
    });

    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(Fairness) {
    bool done = false;

    RunScheduler(4, [&done]() {
      static const size_t kIterations = 10000;

      fibers::Channel<int> xs{5};
      fibers::Channel<int> ys{5};

      xs.Send(1);
      ys.Send(2);

      int balance = 0;

      for (size_t i = 0; i < kIterations; ++i) {
        auto selected_value = fibers::TrySelect(xs, ys);
        switch (selected_value.index()) {
          case 0:
            ASSERT_EQ(std::get<0>(selected_value), 1);
            xs.Send(1);
            ++balance;
            break;
          case 1:
            ASSERT_EQ(std::get<1>(selected_value), 2);
            ys.Send(2);
            --balance;
            break;
        }

        ASSERT_TRUE(std::abs(balance) < 1024);
      }

      done = true;
    });

    ASSERT_TRUE(done);
  }
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(Select) {
  SIMPLE_TEST(NonBlocking1) {
    bool done = false;

    RunScheduler(4, [&done]() {
      fibers::Channel<int> xs{4};
      fibers::Channel<std::string> ys{4};

      {
        xs.Send(17);

        auto selected_value = fibers::Select(xs, ys);

        ASSERT_EQ(selected_value.index(), 0);
        ASSERT_EQ(std::get<0>(selected_value), 17);
      }

      {
        ys.Send("Hi");

        auto selected_value = fibers::Select(xs, ys);

        ASSERT_EQ(selected_value.index(), 1);
        ASSERT_EQ(std::get<1>(selected_value), "Hi");
      }

      done = true;
    });

    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(NonBlocking2) {
    bool done = false;

    RunScheduler(4, [&done]() {
      fibers::Channel<int> xs{4};
      fibers::Channel<std::string> ys{4};

      xs.Send(44);
      ys.Send("Test");

      bool int_received = false;
      bool str_received = false;

      for (size_t i = 0; i < 2; ++i) {
        auto selected_value = fibers::Select(ys, xs);

        if (selected_value.index() == 0) {
          ASSERT_EQ(std::get<0>(selected_value), "Test");
          str_received = true;
        } else if (selected_value.index() == 1) {
          ASSERT_EQ(std::get<1>(selected_value), 44);
          int_received = true;
        } else {
          FAIL_TEST("Impossible");
        }
      }

      ASSERT_TRUE(int_received && str_received);

      done = true;
    });

    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(TwoValues) {
    bool done = false;

    RunScheduler(4, [&done]() {
      fibers::Channel<int> xs{4};
      fibers::Channel<std::string> ys{4};

      fibers::Go([xs]() mutable {
        std::this_thread::sleep_for(1s);
        xs.Send(44);
      });

      fibers::Go([ys]() mutable {
        std::this_thread::sleep_for(2s);
        ys.Send("Test");
      });

      bool int_received = false;
      bool str_received = false;

      for (size_t i = 0; i < 2; ++i) {
        auto selected_value = fibers::Select(ys, xs);

        if (selected_value.index() == 0) {
          ASSERT_EQ(std::get<0>(selected_value), "Test");
          str_received = true;
        } else if (selected_value.index() == 1) {
          ASSERT_EQ(std::get<1>(selected_value), 44);
          int_received = true;
        } else {
          FAIL_TEST("Impossible");
        }
      }

      ASSERT_TRUE(int_received && str_received);

      done = true;
    });

    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(Blocking) {
    wheels::ProcessCPUTimer cpu_timer;

    RunScheduler(4, []() {
      fibers::Channel<int> xs{3};
      fibers::Channel<int> ys{3};

      fibers::Go([xs]() mutable {
        std::this_thread::sleep_for(2s);
        xs.Send(1);
      });

      fibers::Go([ys]() mutable {
        std::this_thread::sleep_for(1s);
        ys.Send(2);
      });

      {
        auto value = fibers::Select(xs, ys);

        ASSERT_EQ(value.index(), 1);
        ASSERT_EQ(std::get<1>(value), 2);
      }

      {
        auto value = fibers::Select(xs, ys);

        ASSERT_EQ(value.index(), 0);
        ASSERT_EQ(std::get<0>(value), 1);
      }
    });

    auto usage = cpu_timer.Elapsed();

    std::cout << "CPU Usage: " << usage.count() << "us" << std::endl;

    ASSERT_TRUE(usage < 128ms);
  }

  SIMPLE_TEST(Fairness) {
    bool done = false;

    RunScheduler(4, [&done]() {
      static const size_t kIterations = 10000;

      fibers::Channel<int> xs{5};
      fibers::Channel<int> ys{5};

      xs.Send(1);
      ys.Send(2);

      int balance = 0;

      for (size_t i = 0; i < kIterations; ++i) {
        auto selected_value = Select(xs, ys);
        switch (selected_value.index()) {
          case 0:
            ASSERT_EQ(std::get<0>(selected_value), 1);
            xs.Send(1);
            ++balance;
            break;
          case 1:
            ASSERT_EQ(std::get<1>(selected_value), 2);
            ys.Send(2);
            --balance;
            break;
        }

        ASSERT_TRUE(std::abs(balance) < 1024);
      }

      done = true;
    });

    ASSERT_TRUE(done);
  }
}

RUN_ALL_TESTS()
