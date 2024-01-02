#include <exe/executors/manual.hpp>
#include <exe/executors/thread_pool.hpp>

#include <exe/gorr/core/task.hpp>
#include <exe/gorr/run/fire.hpp>
#include <exe/gorr/run/teleport.hpp>
#include <exe/gorr/run/yield.hpp>

#include <wheels/test/framework.hpp>

#include <iostream>

using namespace exe;

TEST_SUITE(GorRoutines) {
  SIMPLE_TEST(Fire) {
    auto done = false;

    auto gorroutine = [&]() -> gorr::Task<> {
      std::cout << "Hi" << std::endl;
      done = true;
      co_return;
    };

    auto task = gorroutine();

    ASSERT_FALSE(done);

    gorr::FireAndForget(std::move(task));

    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(Teleport) {
    executors::ManualExecutor scheduler;

    bool done = false;

    auto gorroutine = [&]() -> gorr::Task<> {
      co_await gorr::TeleportTo(scheduler);

      done = true;
    };

    gorr::FireAndForget(gorroutine());

    ASSERT_FALSE(done);

    ASSERT_TRUE(scheduler.RunNext());

    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(Yield) {
    executors::ManualExecutor scheduler;

    bool done = false;

    auto gorroutine = [&]() -> gorr::Task<> {
      co_await gorr::TeleportTo(scheduler);

      for (size_t i = 0; i < 10; ++i) {
        co_await gorr::Yield(scheduler);
      }

      done = true;
    };

    auto task = gorroutine();

    gorr::FireAndForget(std::move(task));

    size_t steps = scheduler.Drain();
    ASSERT_EQ(steps, 1 + 10);

    ASSERT_TRUE(done);
  }
}

RUN_ALL_TESTS()
