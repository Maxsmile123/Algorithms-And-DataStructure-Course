#pragma once

#include <cstdlib>
#include <utility>
#include <functional>

//#include <common/IConteiner.hpp>

// https://en.cppreference.com/w/cpp/utility/functional/less
template <
  typename Key,
  typename Value,
  typename Compare = std::less<Key>
> // https://en.cppreference.com/w/cpp/container/map
class Map {
public:
  Map() {
    // Not implemented
  }

  Map(const Map& /*mp*/) {
    // Not implemented
  }

  Value& operator[](const Key& /*key*/) {
    // Not implemented
  }

  bool IsEmpty() const {
    // Not implemented
  }

  size_t Size() const {
    // Not implemented
  }

  void Swap(Map& /*a*/) {
    // Not implemented
  }

  std::pair<const Key, Value>* Values() const {
    // Not implemented
  }

  void Insert(const std::pair<const Key, Value>& /*val*/){
    // Not implemented
  }

  void Insert(std::pair<const Key, Value>&& /*val*/){
    // Not implemented
  }

  void Erase(const Key& /*key*/){
    // Not implemented
  }

  void Clear() noexcept {
    // Not implemented
  }

  size_t Count(const Key& /*key*/) const {
    // Not implemented
  }

  bool Find(const Key& /*key*/) const {
    // Not implemented
  }

  ~Map() {
    // Not implemented
  }

private:
  template<typename T, typename U>
  class Node {


    private:
      /*???*/
  };
  /*???*/

private:
  /*???*/

};