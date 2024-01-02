#include "../../shared_ptr.hpp"

#include <wheels/test/framework.hpp>

struct Widget {
  Widget(int v) : data(v) {}

  int data;
};

TEST_SUITE(SharedPtr) {
  SIMPLE_TEST(Basics) {
    auto sp = MakeShared<Widget>(7);

    ASSERT_TRUE(sp);
    // Access data
    ASSERT_EQ((*sp).data, 7);
    ASSERT_EQ(sp->data, 7);

    sp.Reset();
    ASSERT_FALSE(sp);
  }

  SIMPLE_TEST(Copy) {
    auto sp1 = MakeShared<Widget>(3);
    auto sp2 = sp1;

    ASSERT_TRUE(sp2);
    ASSERT_EQ(sp2->data, 3);

    SharedPtr<Widget> sp3(sp2);

    ASSERT_TRUE(sp3);
    ASSERT_EQ(sp3->data, 3);
  }

  SIMPLE_TEST(Move) {
    auto sp1 = MakeShared<Widget>(2);
    auto sp2 = std::move(sp1);
    ASSERT_FALSE(sp1);
    ASSERT_TRUE(sp2);
    ASSERT_EQ(sp2->data, 2);
  }

  SIMPLE_TEST(Share) {
    auto sp1 = MakeShared<Widget>(3);
    auto sp2 = sp1;

    ASSERT_TRUE(sp2);
    ASSERT_EQ(sp2->data, 3);

    auto sp3 = std::move(sp1);

    ASSERT_TRUE(sp3);
    ASSERT_FALSE(sp1);

    ASSERT_EQ(sp3->data, 3);

    auto sp4 = sp3;
    sp3.Reset();

    ASSERT_TRUE(sp4);
    ASSERT_EQ(sp4->data, 3);
  }

  SIMPLE_TEST(NullPtrs) {
    SharedPtr<Widget> sp0;
    ASSERT_FALSE(sp0);
    auto sp1 = sp0;
    SharedPtr<Widget> sp2(sp0);
    SharedPtr<Widget> sp3(std::move(sp0));

    ASSERT_FALSE(sp1);
    ASSERT_FALSE(sp2);
    ASSERT_FALSE(sp3);
  }

  struct ListNode {
    ListNode() = default;

    explicit ListNode(SharedPtr<ListNode> _next) : next(_next) {
    }

    SharedPtr<ListNode> next;
  };

  size_t ListLength(SharedPtr<ListNode> head) {
    size_t length = 0;
    while (head) {
      ++length;
      head = head->next;
    }
    return length;
  }

  SIMPLE_TEST(List) {
    SharedPtr<ListNode> head;
    {
      auto n1 = MakeShared<ListNode>();
      auto n2 = MakeShared<ListNode>(n1);
      auto n3 = MakeShared<ListNode>(n2);
      head = n3;
    }

    ASSERT_EQ(ListLength(head), 3);
  }
}

TEST_SUITE(AtomicSharedPtr) {
  SIMPLE_TEST(JustWorks) {
    AtomicSharedPtr<Widget> asp;

    auto sp0 = asp.Load();
    ASSERT_FALSE(sp0);

    auto sp1 = MakeShared<Widget>(8);

    asp.Store(sp1);

    auto sp2 = asp.Load();

    ASSERT_TRUE(sp1);
    ASSERT_TRUE(sp2);

    ASSERT_EQ(sp2->data, 8);

    auto sp3 = MakeShared<Widget>(9);

    bool success = asp.CompareExchangeWeak(sp1, sp3);
    ASSERT_TRUE(success);

    auto sp4 = asp.Load();
    ASSERT_EQ(sp4->data, sp3->data);

    auto sp5 = MakeShared<Widget>(7);
    bool failure = asp.CompareExchangeWeak(sp1, sp5);
    ASSERT_FALSE(failure);

    auto sp6 = asp.Load();
    ASSERT_EQ(sp6->data, sp3->data);
  }

  SIMPLE_TEST(Dtor) {
    auto sp = MakeShared<Widget>(5);
    AtomicSharedPtr<Widget> asp;
    asp.Store(sp);
    sp.Reset();
    ASSERT_FALSE(sp);
  }

  SIMPLE_TEST(TwoAsps) {
    AtomicSharedPtr<Widget> asp1;
    AtomicSharedPtr<Widget> asp2;

    SharedPtr<Widget> sp0;
    auto sp1 = MakeShared<Widget>(1);

    asp1.Store(sp1);
    asp2.CompareExchangeWeak(sp0, sp1);

    auto sp2 = asp1.Load();
    auto sp3 = asp2.Load();

    ASSERT_EQ(sp2->data, sp1->data);
    ASSERT_EQ(sp3->data, sp2->data);
  }

  SIMPLE_TEST(CompareExchangeSamePtrs) {
    AtomicSharedPtr<Widget> asp;

    auto sp1 = MakeShared<Widget>(8);
    auto sp2 = sp1;

    asp.Store(sp1);
    while (!asp.CompareExchangeWeak(sp1, sp2)) {
    }
  }

  TEST(OverflowStamp, wheels::test::TestOptions().AdaptTLToSanitizer()) {
    auto sp = MakeShared<Widget>(11);
    AtomicSharedPtr<Widget> asp;
    asp.Store(sp);

    std::vector<SharedPtr<Widget>> sps;

    for (size_t i = 0; i < 1'000'000; ++i) {
      sps.push_back(asp.Load());
    }
  }
}

RUN_ALL_TESTS()
