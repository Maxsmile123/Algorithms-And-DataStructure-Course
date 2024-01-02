#include <exe/executors/thread_pool.hpp>
#include <exe/executors/manual.hpp>

#include <exe/futures/make/value.hpp>
#include <exe/futures/make/failure.hpp>
#include <exe/futures/make/submit.hpp>
#include <exe/futures/make/just.hpp>

#include <exe/futures/combine/seq/map.hpp>
#include <exe/futures/combine/seq/and_then.hpp>
#include <exe/futures/combine/seq/or_else.hpp>
#include <exe/futures/combine/seq/flatten.hpp>
#include <exe/futures/combine/seq/flat_map.hpp>
#include <exe/futures/combine/seq/via.hpp>

#include <exe/futures/run/get.hpp>

#include <exe/result/make/ok.hpp>
#include <exe/result/make/err.hpp>

#include <wheels/test/framework.hpp>

#include "guard.hpp"

#include <thread>
#include <chrono>

using namespace exe;

// clang-format off

TEST_SUITE(AllocFreeFutures) {
  SIMPLE_TEST(AllocationCount) {
    {
      size_t before = AllocationCount();

      int* ptr = new int{42};

      size_t after = AllocationCount();

      size_t count = after - before;
      ASSERT_EQ(count, 1);

      delete ptr;
    }
  }

  SIMPLE_TEST(Value) {
    {
      AllocationGuard do_not_alloc;

      auto r = futures::Value(1) | futures::Get();

      ASSERT_TRUE(r);
      ASSERT_EQ(*r, 1);
    }
  }

  SIMPLE_TEST(Submit) {
    executors::ThreadPool pool{4};
    pool.Start();

    {
      AllocationGuard do_not_alloc;

      auto f = futures::Submit(pool, [] {
        return result::Ok(17);
      });

      auto r = std::move(f) | futures::Get();

      ASSERT_TRUE(r);
      ASSERT_EQ(*r, 17);
    }

    pool.Stop();
  }

  SIMPLE_TEST(Monad) {
    executors::ThreadPool pool{4};
    pool.Start();

    {
      AllocationGuard do_not_alloc;

      auto f = futures::Just()
               | futures::Via(pool)
               | futures::AndThen([](Unit) {
                   return result::Ok(1);
                 })
               | futures::Map([](int v) {
                   return v + 1;
                 })
               | futures::OrElse([](Error) {
                   return result::Ok(13);
                 });

      auto r = std::move(f) | futures::Get();

      ASSERT_TRUE(r);
      ASSERT_EQ(*r, 2);
    }

    pool.Stop();
  }


  SIMPLE_TEST(Pipeline) {
    executors::ThreadPool pool{4};
    pool.Start();

    {
      AllocationGuard do_not_alloc;

      auto f = futures::Just()
               | futures::Via(pool)
               | futures::AndThen([](Unit) {
                   return result::Ok(1);
                 })
               | futures::Map([](int v) {
                   return v + 1;
                 })
               | futures::OrElse([](Error) {
                   return result::Ok(13);
                 })
               | futures::FlatMap([&pool](int v) {
                   return futures::Submit(pool, [v] {
                     return result::Ok(v + 1);
                   });
                 })
               | futures::Map([](int v) {
                   return v + 1;
                 });

      auto r = std::move(f) | futures::Get();

      ASSERT_TRUE(r);
      ASSERT_EQ(*r, 4);
    }

    pool.Stop();
  }
}

RUN_ALL_TESTS()
