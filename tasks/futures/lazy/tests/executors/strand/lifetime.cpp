#include <exe/executors/thread_pool.hpp>
#include <exe/executors/strand.hpp>
#include <exe/executors/submit.hpp>

#include <exe/threads/wait_group.hpp>

#include <course/twist/test.hpp>

#include <twist/test/repeat.hpp>

#include <fmt/core.h>

#include <memory>

using namespace exe;

//////////////////////////////////////////////////////////////////////

class Automaton : public std::enable_shared_from_this<Automaton> {
 public:
  Automaton(executors::IExecutor& executor, threads::WaitGroup& wg)
      : strand_(executor), wg_(wg) {
  }

  void AsyncMutate() {
    executors::Submit(strand_, [this, self = shared_from_this()] {
      Mutate();
      wg_.Done();
    });
  }

 private:
  void Mutate() {
    state_ = 1 - state_;
  }

 private:
  executors::Strand strand_;
  threads::WaitGroup& wg_;
  int state_ = 0;
};

//////////////////////////////////////////////////////////////////////

void StressTest() {
  executors::ThreadPool pool{4};
  pool.Start();

  twist::test::Repeat repeat;

  while (repeat()) {
    threads::WaitGroup wg;

    auto automaton = std::make_shared<Automaton>(pool, wg);

    const size_t mutations = 1 + repeat.Iter() % 5;

    wg.Add(mutations);

    for (size_t i = 0; i < mutations; ++i) {
      automaton->AsyncMutate();
    }

    automaton.reset();

    wg.Wait();
  }

  fmt::println("Iterations: {}", repeat.IterCount());

  pool.Stop();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(Strand) {
  TWIST_TEST(Automaton, 5s) {
    StressTest();
  }
}

RUN_ALL_TESTS();
