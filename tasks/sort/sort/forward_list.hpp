#pragma once

#include <cstdlib>
#include <iterator>
#include <functional>
#include <utility>

#include <fmt/core.h>


template <typename T>
class ForwardList{
private:
  class Node{
    friend class ForwardListIterator;
    friend class ForwardList;
    /*???*/

    private:
      /*???*/

  };
public:
  class ForwardListIterator{
    public:
    /*???*/

  private:
      ForwardListIterator(const Node*) {
          // Not implemented
      }
  private:
      Node* current;
  };

public:
  ForwardList() {
    // Not implemented
  }

  explicit ForwardList(size_t /*sz*/) {
    // Not implemented
  }

  ForwardList(const std::initializer_list<T>& /*values*/) {
    // Not implemented
  }

  ForwardList(const ForwardList& /*other*/) {
    // Not implemented
  }

  ForwardList& operator=(const ForwardList& /*other*/) {
    std::abort(); // Not implemented
  }

  ForwardListIterator Begin() const noexcept {
    std::abort(); // Not implemented
  }

  ForwardListIterator End() const noexcept {
    std::abort(); // Not implemented
  }

  inline T& Front() const {
    std::abort(); // Not implemented
  }

  inline bool IsEmpty() const noexcept {
    std::abort(); // Not implemented
  }

  inline size_t Size() const noexcept {
    std::abort(); // Not implemented
  }

  void Swap(ForwardList& /*a*/) {
    // Not implemented
  }

  void EraseAfter(ForwardListIterator /*pos*/) {
    // Not implemented
  }

  void InsertAfter(ForwardListIterator /*pos*/, const T& /*value*/) {
    // Not implemented
  }

  ForwardListIterator Find(const T& /*value*/) const {
    std::abort(); // Not implemented
  }

  void Clear() noexcept {
    // Not implemented
  }

  void PushFront(const T& /*value*/) {
    // Not implemented
  }

  void PopFront() {
    // Not implemented
  }

  ~ForwardList() {
    // Not implemented
  }

private:
  /*???*/
};


namespace std {
  // Global swap overloading
  template <typename T>
  void swap(ForwardList<T>& a, ForwardList<T>& b) {
    a.Swap(b);
  }
}
