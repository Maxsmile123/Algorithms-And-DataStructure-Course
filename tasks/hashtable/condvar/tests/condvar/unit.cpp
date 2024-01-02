#include "../../condvar.hpp"

#include <wheels/test/framework.hpp>

#include <twist/test/cpu_timer.hpp>

#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

TEST_SUITE(CondVar) {

  class Event {
   public:
    void Await() {
      std::unique_lock lock(mutex_);
      while (!set_) {
        set_cond_.Wait(lock);
      }
    }

    void Set() {
      std::lock_guard guard(mutex_);
      set_ = true;
      set_cond_.NotifyOne();
    }

    void Reset() {
      std::lock_guard guard(mutex_);
      set_ = false;
    }

   private:
    bool set_{false};
    std::mutex mutex_;
    stdlike::CondVar set_cond_;
  };

  SIMPLE_TEST(NotifyOne) {
    Event pass;

    for (size_t i = 0; i < 3; ++i) {
      pass.Reset();

      bool passed = false;

      std::thread waiter([&]() {
          {
            twist::test::ThreadCPUTimer cpu_timer;
            pass.Await();
            ASSERT_TRUE(cpu_timer.Spent() < 200ms);
          }
          passed = true;
      });

      std::this_thread::sleep_for(1s);

      ASSERT_FALSE(passed);

      pass.Set();
      waiter.join();

      ASSERT_TRUE(passed);
    }
  }

  class Latch {
   public:
    void Await() {
      std::unique_lock lock(mutex_);
      while (!released_) {
        released_cond_.Wait(lock);
      }
    }

    void Release() {
      std::lock_guard guard(mutex_);
      released_ = true;
      released_cond_.NotifyAll();
    }

    void Reset() {
      std::lock_guard guard(mutex_);
      released_ = false;
    }

   private:
    bool released_{false};
    std::mutex mutex_;
    stdlike::CondVar released_cond_;
  };

  SIMPLE_TEST(NotifyAll) {
    Latch latch;

    for (size_t i = 0; i < 3; ++i) {
      latch.Reset();

      std::atomic<size_t> passed{0};

      auto wait_routine = [&]() {
        latch.Await();
        ++passed;
      };

      std::thread t1(wait_routine);
      std::thread t2(wait_routine);

      std::this_thread::sleep_for(1s);

      ASSERT_EQ(passed.load(), 0);

      latch.Release();

      t1.join();
      t2.join();

      ASSERT_EQ(passed.load(), 2);
    }
  }

  SIMPLE_TEST(NotifyManyTimes) {
    static const size_t kIterations = 1000'000;

    stdlike::CondVar cv;
    for (size_t i = 0; i < kIterations; ++i) {
      cv.NotifyOne();
    }
  }
}

RUN_ALL_TESTS()
