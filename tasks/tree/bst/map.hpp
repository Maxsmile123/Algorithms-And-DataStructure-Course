#pragma once

#include <cstdlib>
#include <functional>
#include <vector>
#include <utility>

#include <fmt/core.h>

template <
  typename Key,
  typename Value,
  typename Compare = std::less<Key>
>
class Map {
public:
  Map() {
    // Not implemented
  }

  Value& operator[](const Key& /*key*/) {
    std::abort(); // Not implemented
  }

  inline bool IsEmpty() const noexcept {
    std::abort(); // Not implemented
  }

  inline size_t Size() const noexcept {
    std::abort(); // Not implemented
  }

  void Swap(Map& a) {
    static_assert(std::is_same<decltype(this->comp), decltype(a.comp)>::value,
                  "The compare function types are different");
    // Not implemented
  }

  std::vector<std::pair<const Key, Value>> Values(bool /*is_increase=true*/) const noexcept {
    std::abort(); // Not implemented
  }

  void Insert(const std::pair<const Key, Value>& /*val*/) {
    // Not implemented
  }

  void Insert(const std::initializer_list<std::pair<const Key, Value>>& /*values*/){
    // Not implemented
  }

  void Erase(const Key& /*key*/) {
    // Not implemented
  }

  void Clear() noexcept {
    // Not implemented
  }

  bool Find(const Key& /*key*/) const {
    std::abort(); // Not implemented
  }

  ~Map() {
    // Not implemented
  }

private:
  class Node {
    friend class Map;


    private:
      /*???*/
  };
  /*???*/

private:
  Compare comp;
  /*???*/

};

namespace std{
// Global swap overloading
  template <typename Key, typename Value>
  void swap(Map<Key, Value>& a, Map<Key, Value>& b) {
    a.Swap(b);
  }
}