#include <fmt/core.h>
#include <gtest/gtest.h>

#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "../vector.hpp"

class Singleton {
public:
    Singleton() = delete;
    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton&& operator=(Singleton&&) = delete;

    static Singleton* getInstance() {
        if (instance == nullptr) {
            instance = new Singleton();
        }
        return instance;
    }

private:
    static Singleton* instance;
};

Singleton* Singleton::instance = nullptr;


class MemoryUseObject {
public:
    MemoryUseObject() {
        a = malloc(100);
    };

    ~MemoryUseObject(){
        free(a);
    }


private:
    void* a;

}



class VectorTest : public testing::Test {
protected:
    void SetUp() override {
        vec.PushBack(1);
        vec.PushBack(2);
        vec.PushBack(3);
        vec.PushBack(4);
        vec.PushBack(5);
        vec.PushBack(6);
        vec.PushBack(7);
        assert(vec.Size() == sz);
    }

    Vector<int> vec;
    const size_t sz = 7;
};

TEST(EmptyVectorTest, DefaultConstructor) {
    Vector<int> vec;
    ASSERT_TRUE(vec.IsEmpty()) << "Default vector isn't empty!";
    ASSERT_EQ(vec.Capacity(), 0) << "Vector should not allocate memory in the default constructor!";
    ASSERT_EQ(vec.Data(), nullptr) << "Vector should not allocate memory in the default constructor!";
}

TEST(EmptyVectorTest, AssignIntConstructor) {
    Vector<int> vec(10, 5);
    ASSERT_EQ(vec.Size(), 10);
    for (size_t i = 0; i < 10; ++i) {
        ASSERT_EQ(vec[i], 5);
    }
}

TEST_F(VectorTest, CopyConstructor) {
    Vector<int> vec1 = vec;
    ASSERT_NE(&vec1, &vec) << "Copy constructor must do copy!\n";
    ASSERT_EQ(vec1.Size(), vec.Size());
    for (size_t i = 0; i < vec.Size(); ++i) {
        ASSERT_EQ(vec1[i], vec[i]) << "Values must be equal!";
    }
}

TEST(EmptyVectorTest, CopyConstructorWithPointers) {
    int a = 1;
    int b = 2;
    int c = 3;
    Vector<int*> vec1;
    vec1.PushBack(&a);
    vec1.PushBack(&b);
    vec1.PushBack(&c);
    Vector<int*> vec = vec1;
    ASSERT_NE(&vec1, &vec) << "Copy constructor must do copy!\n";
    ASSERT_EQ(vec1.Size(), vec.Size());
    for (size_t i = 0; i < vec.Size(); ++i) {
        ASSERT_EQ(*vec1[i], *vec[i]) << "Values must be equal!";
        ASSERT_EQ(vec1[i], vec[i]) << "Need copy!";
    }
}

TEST_F(VectorTest, MoveConstructor) {
    Vector<int> vec1 = std::move(vec);
    ASSERT_EQ(vec1.Size(), sz);
    for (size_t i = 0; i < vec.Size(); ++i) {
        ASSERT_EQ(vec1[i], i + 1);
        ASSERT_EQ(vec[i], 0);
    }
}

TEST(EmptyVectorTest, CopyOperator) {
    Vector<MemoryUseObject> vec1;
    vec1.PushBack(MemoryUseObject());
    Vector<MemoryUseObject> vec;
    vec1 = vec;
    ASSERT_NE(&vec1, &vec) << "Copy constructor must do copy!\n";
    ASSERT_EQ(vec1.Size(), vec.Size());
}

TEST(EmptyVectorTest, MoveOperator) {
    Vector<MemoryUseObject> vec1;
    vec1.PushBack(MemoryUseObject());
    Vector<MemoryUseObject> vec;
    vec1 = std::move(vec);
    ASSERT_EQ(vec1.Size(), 1);
    ASSERT_EQ(vec.Size(), 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}