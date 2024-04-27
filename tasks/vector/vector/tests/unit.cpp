#include "../vector.hpp"
#include "../vector.cpp"

#include <fmt/core.h>
#include <gtest/gtest.h>

#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <memory>

class Singleton {
private:
    Singleton() {}

public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

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

    MemoryUseObject(const MemoryUseObject&) {
        a = malloc(100);
    };

    MemoryUseObject(MemoryUseObject&& other) {
        a = other.a;
        other.a = nullptr;
    };

    MemoryUseObject& operator=(const MemoryUseObject&){
        return *this;
    }

     MemoryUseObject& operator=(MemoryUseObject&& other){
        if (a) {
            free(a);
        }
        a = other.a;
        other.a = nullptr;
        return *this;
    }

    ~MemoryUseObject(){
        free(a);
    }


private:
    void* a;
};


struct President {
    std::string name;
    std::string country;
    int year;
    
    President(std::string p_name, std::string p_country, int p_year)
        : name(std::move(p_name)), country(std::move(p_country)), year(p_year)
    {}
    
    President(President&& other)
        : name(std::move(other.name)), country(std::move(other.country)), year(other.year)
    {}
    
    President& operator=(const President& other) = default;
};

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
    vec = std::move(vec1);
    ASSERT_EQ(vec.Size(), 1);
    ASSERT_EQ(vec1.Size(), 0);
}

TEST(EmptyVectorTest, Init_list) {
    Vector<int> vec({1, 2, 3, 4, 5, 6, 7, 8, 9});
    for (size_t i = 0; i < vec.Size(); ++i) {
        ASSERT_EQ(vec[i], i + 1);
    }
}

TEST(EmptyVectorTest, MoveToPushBack) {
    Vector<std::unique_ptr<MemoryUseObject>> vec;
    std::unique_ptr<MemoryUseObject> ptr = std::make_unique<MemoryUseObject>();
    vec.PushBack(std::move(ptr));
    vec.PopBack(); // if work not correct will error with ASAN
}

TEST(EmptyVectorTest, JustReserve) {
    Vector<int> vec;
    vec.Reserve(100);
    ASSERT_EQ(vec.Capacity(), 100);
    ASSERT_EQ(vec.Size(), 0);
    for (size_t i = 0; i < 99; ++i) {
        vec.PushBack(1);
    }
    ASSERT_EQ(vec.Capacity(), 100);
    ASSERT_EQ(vec.Size(), 99);
}

TEST(EmptyVectorTest, ReserveWithRealloc) {
    Vector<int> vec({1, 2, 3, 4, 5, 6, 7, 8, 9});
    vec.Reserve(100);
    ASSERT_EQ(vec.Capacity(), 100);
    ASSERT_EQ(vec.Size(), 9);
    for (size_t i = 0; i < vec.Size(); ++i) {
        ASSERT_EQ(vec[i], i + 1);
    }
}

TEST(EmptyVectorTest, ReserveWithNoEffect) {
    Vector<int> vec({1, 2, 3, 4, 5, 6, 7, 8, 9});
    vec.Reserve(1);
    ASSERT_EQ(vec.Capacity(), 10);
    ASSERT_EQ(vec.Size(), 9);
    for (size_t i = 0; i < vec.Size(); ++i) {
        ASSERT_EQ(vec[i], i + 1);
    }
}

TEST(EmptyVectorTest, OperatorSqueareBrackets) {
    Vector<std::unique_ptr<std::mutex>> vec;
    auto ptr = std::make_unique<std::mutex>();
    vec.PushBack(std::move(ptr));

    std::thread t1([&](){
        vec[0]->lock();
    });

    std::thread t2([&](){
        vec.Front()->unlock();
    });

    std::thread t3([&](){
        vec.Back()->lock();
    });

    t1.join();
    t2.join();

    auto future = std::async(std::launch::async, &std::thread::join, &t3);
    ASSERT_LT(
        future.wait_for(std::chrono::seconds(1)),
        std::future_status::timeout
    ) << "There is deadlock!\n"; 
}

TEST(EmptyVectorTest, VectorEmplaceBack) {
    Vector<President> vec;
    std::string name = "Nelson Mandela";
    vec.EmplaceBack(name, "South Africa", 1994);
    ASSERT_FALSE(name.empty());


    vec.EmplaceBack("Franklin Delano Roosevelt", "USA", 1936);

    ASSERT_EQ(vec.Size(), 2);
    ASSERT_EQ(vec[0].year, 1994);
    ASSERT_EQ(vec[1].year, 1936);
}

TEST(EmptyVectorTest, VoidAsTemplate) {
    Vector<void*> vec;
    vec.PushBack(malloc(1));
    vec.PushBack(malloc(1));
    vec.PushBack(malloc(1));
    vec.PushBack(malloc(1));
    vec.PushBack(malloc(1));
}


TEST_F(VectorTest, CopyConstructor) {
    Vector<int> vec1 = vec;
    ASSERT_NE(&vec1, &vec) << "Copy constructor must do copy!\n";
    ASSERT_EQ(vec1.Size(), vec.Size());
    for (size_t i = 0; i < vec.Size(); ++i) {
        ASSERT_EQ(vec1[i], vec[i]) << "Values must be equal!";
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

TEST_F(VectorTest, RawData) {
    auto data = vec.Data();
    for (size_t i = 0; i < vec.Size(); ++i) {
        ASSERT_EQ(*(data + i), i + 1);
    }
}


TEST_F(VectorTest, VectorClear) {
    size_t old_cap = vec.Capacity();
    vec.Clear();
    ASSERT_EQ(vec.Capacity(), old_cap);
    ASSERT_EQ(vec.Size(), 0);
}

TEST_F(VectorTest, InsertFront) {
    vec.Insert(0, 0);
    ASSERT_EQ(vec.Size(), sz + 1);
    for (size_t i = 0; i < vec.Size(); ++i) {
        ASSERT_EQ(vec[i], i);
    }
}

TEST_F(VectorTest, InsertBack) {
    vec.Insert(sz, sz + 1);
    ASSERT_EQ(vec.Size(), sz + 1);
    for (size_t i = 0; i < vec.Size(); ++i) {
        ASSERT_EQ(vec[i], i + 1);
    }
}

TEST_F(VectorTest, InsertMid) {
    vec.Insert(sz / 2, 0);
    ASSERT_EQ(vec.Size(), sz + 1);
    for (size_t i = 0; i < vec.Size(); ++i) {
        if (i == sz / 2) {
            ASSERT_EQ(vec[i], 0);
        } else if (i < sz / 2) {
            ASSERT_EQ(vec[i], i + 1);
        } else {
            ASSERT_EQ(vec[i], i);
        }
    }
}

TEST_F(VectorTest, InsertWithResize) {
    size_t cur_cap = vec.Capacity();
    for (size_t i = sz; i < cur_cap; ++i) {
        vec.PushBack(i + 1);
    }

    size_t pos = vec.Size() / 2;
    vec.Insert(pos, 0);
    ASSERT_NE(cur_cap, vec.Capacity());
    for (size_t i = 0; i < vec.Size(); ++i) {
        if (i == vec.Size() / 2) {
            ASSERT_EQ(vec[i], 0);
        } else if (i < vec.Size() / 2) {
            ASSERT_EQ(vec[i], i + 1);
        } else {
            ASSERT_EQ(vec[i], i);
        }
    }
}

TEST_F(VectorTest, VectorPopBack) {
    vec.PopBack();
    ASSERT_EQ(vec.Size(), sz - 1);
    for (size_t i = 0; i < vec.Size(); ++i) {
        ASSERT_EQ(vec[i], i + 1);
    }
}

TEST_F(VectorTest, VectorEraseAll) {
    size_t old_cap = vec.Capacity();
    vec.Erase(0, sz);
    ASSERT_EQ(vec.Capacity(), old_cap);
    ASSERT_EQ(vec.Size(), 0);
}

TEST_F(VectorTest, VectorEraseFront) {
    size_t old_cap = vec.Capacity();
    vec.Erase(0, 1);
    ASSERT_EQ(vec.Capacity(), old_cap);
    ASSERT_EQ(vec.Size(), sz - 1);
    for (size_t i = 0; i < vec.Size(); ++i) {
        ASSERT_EQ(vec[i], i + 2);
    }
}

TEST_F(VectorTest, VectorEraseBack) {
    size_t old_cap = vec.Capacity();
    vec.Erase(sz - 1, sz);
    ASSERT_EQ(vec.Capacity(), old_cap);
    ASSERT_EQ(vec.Size(), sz - 1);
    for (size_t i = 0; i < vec.Size(); ++i) {
        ASSERT_EQ(vec[i], i + 1);
    }
}

TEST_F(VectorTest, VectorEraseMid) {
    size_t old_cap = vec.Capacity();
    std::vector<int> a = {1, 2, 5, 6, 7};
    vec.Erase(sz / 2 - 1, sz / 2 + 1); // 2 - 4
    ASSERT_EQ(vec.Capacity(), old_cap);
    ASSERT_EQ(vec.Size(), sz - 2);
    for (size_t i = 0; i < vec.Size(); ++i) {
        ASSERT_EQ(vec[i], a[i]);
    }
}

TEST_F(VectorTest, VectorEraseNoneExistingPositions) {
    size_t old_cap = vec.Capacity();
    vec.Erase(sz + 1, sz + 3); // no effect
    ASSERT_EQ(vec.Capacity(), old_cap);
    ASSERT_EQ(vec.Size(), sz);
    for (size_t i = 0; i < vec.Size(); ++i) {
        ASSERT_EQ(vec[i], i + 1);
    }
}

TEST_F(VectorTest, VectorResizeGreaterThenCurrent) {
    size_t old_cap = vec.Capacity();
    size_t old_size = vec.Size();
    vec.Resize(old_size + old_cap, 0);
    ASSERT_NE(vec.Capacity(), old_cap);
    ASSERT_EQ(vec.Size(), old_size + old_cap);
    for (size_t i = 0; i < old_size; ++i) {
        ASSERT_EQ(vec[i], i + 1);
    }
    for (size_t i = old_size; i < vec.Size(); ++i) {
        ASSERT_EQ(vec[i], 0);
    }
}

TEST_F(VectorTest, VectorResizeEqualCurrent) {
    size_t old_cap = vec.Capacity();
    size_t old_size = vec.Size();
    vec.Resize(old_size, 0); // no effect
    ASSERT_EQ(vec.Capacity(), old_cap);
    ASSERT_EQ(vec.Size(), old_size);
    for (size_t i = 0; i < old_size; ++i) {
        ASSERT_EQ(vec[i], i + 1);
    }
}

TEST_F(VectorTest, VectorResizeLessThenCurrent) {
    size_t old_cap = vec.Capacity();
    size_t old_size = vec.Size();
    vec.Resize(old_size - 4, 0); // reducing
    ASSERT_EQ(vec.Capacity(), old_cap);
    ASSERT_EQ(vec.Size(), old_size - 4);
    for (size_t i = 0; i < old_size - 4; ++i) {
        ASSERT_EQ(vec[i], i + 1);
    }
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}