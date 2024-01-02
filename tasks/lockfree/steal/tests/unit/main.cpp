#include "../../work_stealing_queue.hpp"

#include <wheels/test/framework.hpp>

//////////////////////////////////////////////////////////////////////

struct TestObject {
  int data;
};

class TestObjectMaker {
 public:
  TestObjectMaker() {
    PrepareNext();
  }

  ~TestObjectMaker() {
    delete next_;
  }

  TestObject* Get() {
    return next_;
  }

  void PrepareNext() {
    next_ = new TestObject{next_value_++};
  }

 private:
  int next_value_ = 0;
  TestObject* next_;
};

//////////////////////////////////////////////////////////////////////

TEST_SUITE(WorkStealingQueue) {

  template <size_t Capacity>
  void Cleanup(WorkStealingQueue<TestObject, Capacity>& queue) {
    while (TestObject* obj = queue.TryPop()) {
      delete obj;
    }
  }

  SIMPLE_TEST(JustWorks) {
    WorkStealingQueue<TestObject, 17> q;

    ASSERT_TRUE(q.TryPush(new TestObject{17}));

    {
      TestObject* obj = q.TryPop();

      ASSERT_TRUE(obj != nullptr);
      ASSERT_EQ(obj->data, 17);

      delete obj;
    }

    ASSERT_EQ(q.TryPop(), nullptr);
  }

  SIMPLE_TEST(Fifo) {
    WorkStealingQueue<TestObject, 128> q;

    for (int i = 0; i < 64; ++i) {
      ASSERT_TRUE(q.TryPush(new TestObject{i}));
    }

    for (int i = 0; i < 64; ++i) {
      TestObject* obj = q.TryPop();

      ASSERT_TRUE(obj != nullptr);
      ASSERT_EQ(obj->data, i);

      delete obj;
    }
  }

  SIMPLE_TEST(Capacity) {
    WorkStealingQueue<TestObject, 33> q;

    for (int i = 0; i < 33; ++i) {
      ASSERT_TRUE(q.TryPush(new TestObject{i}));
    }

    {
      TestObject* overflow = new TestObject{34};

      ASSERT_FALSE(q.TryPush(overflow));
      ASSERT_FALSE(q.TryPush(overflow));

      delete overflow;
    }

    Cleanup(q);
  }

  SIMPLE_TEST(Wrap) {
    WorkStealingQueue<TestObject, 12> q;

    for (int i = 0; i < 6; ++i) {
      ASSERT_TRUE(q.TryPush(new TestObject{i}));
    }

    for (int i = 6; i < 256; ++i) {
      ASSERT_TRUE(q.TryPush(new TestObject{i}));
      {
        TestObject* obj = q.TryPop();

        ASSERT_TRUE(obj != nullptr);
        ASSERT_EQ(obj->data, i - 6);

        delete obj;
      }
    }

    Cleanup(q);
  }

  SIMPLE_TEST(Grab) {
    WorkStealingQueue<TestObject, 17> q;

    for (int i = 0; i < 7; ++i) {
      q.TryPush(new TestObject{i});
    }

    TestObject* buffer[5];

    {
      size_t count = q.Grab({buffer, 5});
      ASSERT_EQ(count, 5);

      for (size_t i = 0; i < count; ++i) {
        delete buffer[i];
      }
    }

    {
      size_t count = q.Grab({buffer, 5});
      ASSERT_EQ(count, 2);

      for (size_t i = 0; i < count; ++i) {
        delete buffer[i];
      }
    }
  }

  SIMPLE_TEST(Grab2) {
    WorkStealingQueue<TestObject, 33> q;

    int produced_cs = 0;
    int consumed_cs = 0;

    TestObjectMaker maker;

    TestObject* buffer[15];

    for (size_t i = 0; i < 100; ++i) {
      for (size_t j = 0; j < 10; ++j) {
        TestObject* obj = maker.Get();
        if (q.TryPush(obj)) {
          produced_cs += obj->data;
          maker.PrepareNext();
        }
      }

      {
        size_t to_grab = 5 + i % 10;
        size_t count = q.Grab({buffer, to_grab});

        ASSERT_TRUE(count > 0);

        for (size_t k = 0; k < count; ++k) {
          consumed_cs += buffer[k]->data;
          delete buffer[k];
        }
      }
    }

    while (TestObject* obj = q.TryPop()) {
      consumed_cs += obj->data;
      delete obj;
    }

    std::cout << "Produced: " << produced_cs << std::endl;
    std::cout << "Consumed: " << consumed_cs << std::endl;

    ASSERT_EQ(produced_cs, consumed_cs);
  }
}

RUN_ALL_TESTS()
