#include <exe/executors/manual.hpp>
#include <exe/executors/execute.hpp>

#include <wheels/test/framework.hpp>

using namespace exe::executors;

TEST_SUITE(Manual) {
  SIMPLE_TEST(JustWorks) {
    ManualExecutor manual;

    size_t step = 0;

    ASSERT_FALSE(manual.HasTasks());

    ASSERT_FALSE(manual.RunNext());
    ASSERT_EQ(manual.RunAtMost(99), 0);

    Execute(manual, [&]() {
      step = 1;
    });

    ASSERT_TRUE(manual.HasTasks());
    ASSERT_EQ(manual.TaskCount(), 1);

    ASSERT_EQ(step, 0);

    Execute(manual, [&]() {
      step = 2;
    });

    ASSERT_EQ(manual.TaskCount(), 2);

    ASSERT_EQ(step, 0);

    ASSERT_TRUE(manual.RunNext());

    ASSERT_EQ(step, 1);

    ASSERT_TRUE(manual.HasTasks());
    ASSERT_EQ(manual.TaskCount(), 1);

    Execute(manual, [&]() {
      step = 3;
    });

    ASSERT_EQ(manual.TaskCount(), 2);

    ASSERT_EQ(manual.RunAtMost(99), 2);
    ASSERT_EQ(step, 3);

    ASSERT_FALSE(manual.HasTasks());
    ASSERT_FALSE(manual.RunNext());
  }

  class Looper {
   public:
    explicit Looper(IExecutor& e, size_t iters)
    : executor_(e), iters_left_(iters) {
    }

    void Iter() {
      --iters_left_;
      if (iters_left_ > 0) {
        Execute(executor_, [this]() {
          Iter();
        });
      }
    }

   private:
    IExecutor& executor_;
    size_t iters_left_;
  };

  SIMPLE_TEST(RunAtMost) {
    ManualExecutor manual;

    Looper looper{manual, 256};

    Execute(manual, [&looper]() {
      looper.Iter();
    });

    size_t tasks = 0;
    do {
      tasks += manual.RunAtMost(7);
    } while (manual.HasTasks());

    ASSERT_EQ(tasks, 256);
  }

  SIMPLE_TEST(Drain) {
    ManualExecutor manual;

    Looper looper{manual, 117};

    Execute(manual, [&looper]() {
      looper.Iter();
    });

    ASSERT_EQ(manual.Drain(), 117);
  }
}

RUN_ALL_TESTS()
