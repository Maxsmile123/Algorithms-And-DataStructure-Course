#pragma once

#include <cstdlib>
#include <functional>
#include <vector>
#include <utility>

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

  Map(const Map& /*mp*/) {
    // Not implemented
  }

  Map& operator=(const Map& /*mp*/) {
    std::abort(); // Not implemented
  }

  Value& operator[](const Key& /*key*/) {
    std::abort(); // Not implemented
  }

  inline bool IsEmpty() const {
    std::abort(); // Not implemented
  }

  inline size_t Size() const {
    std::abort(); // Not implemented
  }

  void Swap(Map& /*a*/) {
    // Not implemented
  }

  std::vector<std::pair<const Key, Value>> Values(bool /*is_increase=true*/) const {
    std::abort(); // Not implemented
  }

  void Insert(const std::pair<const Key, Value>& /*val*/){
    // Not implemented
  }

  void Insert(const std::initializer_list<std::pair<const Key, Value>>& /*values*/){
    // Not implemented
  }

  void Insert(std::pair<const Key, Value>&& /*val*/){
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


    private:
      /*???*/
  };
  /*???*/

private:
  /*???*/

};

// Global swap overloading
template <typename Key, typename Value>
void swap(Map<Key, Value>& a, Map<Key, Value>& b){
  a.Swap(b);
}