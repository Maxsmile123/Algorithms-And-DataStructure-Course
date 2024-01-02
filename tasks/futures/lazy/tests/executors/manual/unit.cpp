#include <exe/executors/manual.hpp>
#include <exe/executors/submit.hpp>

#include <wheels/test/framework.hpp>

using namespace exe;

TEST_SUITE(Manual) {
  SIMPLE_TEST(JustWorks) {
    executors::ManualExecutor manual;

    size_t step = 0;

    ASSERT_TRUE(manual.IsEmpty());
    ASSERT_FALSE(manual.NonEmpty());

    ASSERT_FALSE(manual.RunNext());
    ASSERT_EQ(manual.RunAtMost(99), 0);

    executors::Submit(manual, [&] {
      step = 1;
    });

    ASSERT_FALSE(manual.IsEmpty());
    ASSERT_TRUE(manual.NonEmpty());
    ASSERT_EQ(manual.TaskCount(), 1);

    ASSERT_EQ(step, 0);

    executors::Submit(manual, [&] {
      step = 2;
    });

    ASSERT_EQ(manual.TaskCount(), 2);

    ASSERT_EQ(step, 0);

    ASSERT_TRUE(manual.RunNext());

    ASSERT_EQ(step, 1);

    ASSERT_FALSE(manual.IsEmpty());
    ASSERT_TRUE(manual.NonEmpty());
    ASSERT_EQ(manual.TaskCount(), 1);

    executors::Submit(manual, [&] {
      step = 3;
    });

    ASSERT_EQ(manual.TaskCount(), 2);

    ASSERT_EQ(manual.RunAtMost(99), 2);
    ASSERT_EQ(step, 3);

    ASSERT_TRUE(manual.IsEmpty());
    ASSERT_FALSE(manual.NonEmpty());
    ASSERT_FALSE(manual.RunNext());
  }

  class Looper {
   public:
    explicit Looper(executors::IExecutor& e, size_t iters)
        : executor_(e),
          iters_left_(iters) {
    }

    void Start() {
      Submit();
    }

    void Iter() {
      --iters_left_;
      if (iters_left_ > 0) {
        Submit();
      }
    }

   private:
    void Submit() {
      executors::Submit(executor_, [this] {
        Iter();
      });
    }

   private:
    executors::IExecutor& executor_;
    size_t iters_left_;
  };

  SIMPLE_TEST(RunAtMost) {
    executors::ManualExecutor manual;

    Looper looper{manual, 256};
    looper.Start();

    size_t tasks = 0;
    do {
      tasks += manual.RunAtMost(7);
    } while (manual.NonEmpty());

    ASSERT_EQ(tasks, 256);
  }

  SIMPLE_TEST(Drain) {
    executors::ManualExecutor manual;

    Looper looper{manual, 117};
    looper.Start();

    ASSERT_EQ(manual.Drain(), 117);
  }
}

RUN_ALL_TESTS()
