#include <exe/executors/thread_pool.hpp>
#include <exe/executors/strand.hpp>
#include <exe/executors/submit.hpp>

#include <course/twist/test.hpp>

#include <twist/test/repeat.hpp>

#include <memory>

using namespace exe;

//////////////////////////////////////////////////////////////////////

class Automaton : public std::enable_shared_from_this<Automaton> {
 public:
  Automaton(executors::IExecutor& executor)
      : strand_(executor) {
  }

  void AsyncMutate() {
    executors::Submit(strand_, [self = shared_from_this()] {
      self->Mutate();
    });
  }

 private:
  void Mutate() {
    state_ = 1 - state_;
  }

 private:
  executors::Strand strand_;
  int state_ = 0;
};

//////////////////////////////////////////////////////////////////////

void StressTest() {
  executors::ThreadPool pool{4};
  pool.Start();

  for (twist::test::Repeat repeat; repeat.Test(); ) {
    auto automaton = std::make_shared<Automaton>(pool);

    size_t mutations = 1 + repeat.Iter() % 5;
    for (size_t i = 0; i < mutations; ++i) {
      automaton->AsyncMutate();
    }

    automaton.reset();

    pool.WaitIdle();
  }

  pool.Stop();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(Strand) {
  TWIST_TEST(Automaton, 5s) {
    StressTest();
  }
}

RUN_ALL_TESTS();
