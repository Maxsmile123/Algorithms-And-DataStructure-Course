#include <exe/executors/thread_pool.hpp>
#include <exe/executors/strand.hpp>
#include <exe/executors/submit.hpp>

#include <course/twist/test.hpp>

#include <twist/test/repeat.hpp>

#include <list>

using namespace exe;
using namespace std::chrono_literals;

/////////////////////////////////////////////////////////////////////

class Robot {
 public:
  explicit Robot(executors::IExecutor& executor)
      : strand_(executor) {
  }

  void Push() {
    executors::Submit(strand_, [this]() {
      Step();
    });
  }

  size_t StepCount() const {
    return steps_;
  }

 private:
  void Step() {
    ++steps_;
  }

 private:
  executors::Strand strand_;
  size_t steps_{0};
};

void Robots(size_t strands, size_t pushes) {
  executors::ThreadPool workers{4};
  workers.Start();

  std::list<Robot> robots;
  for (size_t i = 0; i < strands; ++i) {
    robots.emplace_back(workers);
  }

  executors::ThreadPool clients{strands};
  clients.Start();

  twist::test::Repeat repeat;

  while (repeat.Test()) {
    for (auto& robot : robots) {
      Submit(clients, [&robot, pushes] {
        for (size_t j = 0; j < pushes; ++j) {
          robot.Push();
        }
      });
    }

    clients.WaitIdle();
    workers.WaitIdle();
  }

  for (auto& robot : robots) {
    ASSERT_EQ(robot.StepCount(), repeat.Iter() * pushes);
  }

  clients.Stop();
  workers.Stop();
}

//////////////////////////////////////////////////////////////////////

void MissingTasks() {
  executors::ThreadPool pool{4};
  pool.Start();

  for (twist::test::Repeat repeat; repeat.Test(); ) {
    executors::Strand strand{pool};

    size_t todo = 2 + repeat.Iter() % 5;

    size_t done = 0;

    for (size_t i = 0; i < todo; ++i) {
      executors::Submit(strand, [&done] {
        ++done;
      });
    }

    pool.WaitIdle();

    ASSERT_EQ(done, todo);
  }

  pool.Stop();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(Strand) {
  TWIST_TEST(Robots_1, 5s) {
    Robots(30, 30);
  }

  TWIST_TEST(Robots_2, 5s) {
    Robots(50, 20);
  }

  TWIST_TEST(MissingTasks, 5s) {
    MissingTasks();
  }
}

RUN_ALL_TESTS()
