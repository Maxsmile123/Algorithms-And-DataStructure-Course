#pragma once

#include <cstdlib>
#include <functional>
#include <iterator>
#include <vector>
#include <utility>

#include <fmt/core.h>

template <
  typename Key,
  typename Value,
  typename Compare = std::less<Key>
>
class Map {

  class Node;
public:
  class MapIterator {
    public:
      // NOLINTNEXTLINE
      using value_type = std::pair<const Key, Value>;
      // NOLINTNEXTLINE
      using reference_type = value_type&;
      // NOLINTNEXTLINE
      using pointer_type = value_type*;
      // NOLINTNEXTLINE
      using difference_type = std::ptrdiff_t;
      // NOLINTNEXTLINE
      using iterator_category = std::forward_iterator_tag;

      inline bool operator==(const MapIterator&) const {
          std::abort(); // Not implemented
      };

      inline bool operator!=(const MapIterator&) const {
          std::abort(); // Not implemented
      };

      inline reference_type operator*() const {
          std::abort(); // Not implemented
      };

      MapIterator& operator++() {
          std::abort(); // Not implemented
      };

      MapIterator operator++(int) {
          std::abort(); // Not implemented
      };

      inline pointer_type operator->() const {
          std::abort(); // Not implemented
      };

  private:
      explicit MapIterator(const Node*) {
          // Not implemented
      }
  private:
      Node* current_;
  };


  inline MapIterator Begin() const noexcept {
    std::abort(); // Not implemented
  }

  inline MapIterator End() const noexcept {
    std::abort(); // Not implemented
  }

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

  MapIterator Find(const Key& /*key*/) const {
    std::abort(); // Not implemented
  }

  ~Map() {
    // Not implemented
  }

private:
  class Node {
    friend class MapIterator;
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