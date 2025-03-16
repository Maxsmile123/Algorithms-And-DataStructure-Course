#pragma once

#include <fmt/core.h>

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <utility>

#include "exceptions.hpp"

template <typename T>
class List {
private:
    class Node {
        friend class ListIterator;
        friend class List;
        /*???*/

    private:
        /*???*/
    };

public:
    class ListIterator {
    public:
        // NOLINTNEXTLINE
        using value_type = T;
        // NOLINTNEXTLINE
        using reference_type = value_type&;
        // NOLINTNEXTLINE
        using pointer_type = value_type*;
        // NOLINTNEXTLINE
        using difference_type = std::ptrdiff_t;
        // NOLINTNEXTLINE
        using iterator_category = std::bidirectional_iterator_tag;

        inline bool operator==(const ListIterator&) const {
            std::abort();  // Not implemented
        };

        inline bool operator!=(const ListIterator&) const {
            std::abort();  // Not implemented
        };

        inline reference_type operator*() const {
            std::abort();  // Not implemented
        };

        ListIterator& operator++() {
            std::abort();  // Not implemented
        };

        ListIterator operator++(int) {
            std::abort();  // Not implemented
        };

        ListIterator& operator--() {
            std::abort();  // Not implemented
        };

        ListIterator operator--(int) {
            std::abort();  // Not implemented
        };

        /*The overload of operator -> must either return a raw pointer,
        or return an object (by reference or by value) for which
        operator -> is in turn overloaded.*/
        inline pointer_type operator->() const {
            std::abort();  // Not implemented
        };

    private:
        explicit ListIterator(const Node*) {
            // Not implemented
        }

    private:
        Node* current_;
    };

public:
    List() {
        // Not implemented
    }

    explicit List(size_t /*sz*/) {
        // Not implemented
    }

    List(const std::initializer_list<T>& /*values*/) {
        // Not implemented
    }

    List(const List& /*other*/) {
        // Not implemented
    }

    List& operator=(const List& /*other*/) {
        std::abort();  // Not implemented
    }

    ListIterator Begin() const noexcept {
        std::abort();  // Not implemented
    }

    ListIterator End() const noexcept {
        std::abort();  // Not implemented
    }

    inline T& Front() const {
        std::abort();  // Not implemented
    }

    inline T& Back() const {
        std::abort();  // Not implemented
    }

    inline bool IsEmpty() const noexcept {
        std::abort();  // Not implemented
    }

    inline size_t Size() const noexcept {
        std::abort();  // Not implemented
    }

    void Swap(List& /*a*/) {
        // Not implemented
    }

    ListIterator Find(const T& /*value*/) const {
        std::abort();  // Not implemented
    }

    void Erase(ListIterator /*pos*/) {
        // Not implemented
    }

    void Insert(ListIterator /*pos*/, const T& /*value*/) {
        // Not implemented
    }

    void Clear() noexcept {
        // Not implemented
    }

    void PushBack(const T& /*value*/) {
        // Not implemented
    }

    void PushFront(const T& /*value*/) {
        // Not implemented
    }

    void PopBack() {
        // Not implemented
    }

    void PopFront() {
        // Not implemented
    }

    ~List() {
        // Not implemented
    }

private:
    /*???*/
};

namespace std {
// Global swap overloading
template <typename T>
// NOLINTNEXTLINE
void swap(List<T>& a, List<T>& b) {
    a.Swap(b);
}
}  // namespace std
