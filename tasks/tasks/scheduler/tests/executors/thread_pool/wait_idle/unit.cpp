#include <exe/executors/thread_pool.hpp>
#include <exe/executors/submit.hpp>

#include <wheels/test/framework.hpp>

#include <chrono>
#include <thread>

using namespace exe;

using namespace std::chrono_literals;

TEST_SUITE(WaitIdle) {
  SIMPLE_TEST(JustWorks) {
    executors::ThreadPool pool{4};
    pool.Start();

    bool done = false;

    executors::Submit(pool, [&] {
      std::this_thread::sleep_for(1s);
      executors::Submit(pool, [&done] {
        done = true;
      });
    });

    pool.WaitIdle();

    ASSERT_TRUE(done);

    pool.Stop();
  }
}

RUN_ALL_TESTS()
