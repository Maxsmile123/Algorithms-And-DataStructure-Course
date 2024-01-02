#pragma once

#include <cstdlib>
#include <utility>

#include <common/IConteiner.hpp>

// https://en.cppreference.com/w/cpp/utility/functional/less
template <
  typename Key,
  typename Value,
  typename Compare = std::less<Key>
> // https://en.cppreference.com/w/cpp/container/map
class Map: public IConteiner {
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

  bool empty() const {
    // Not implemented
  }

  size_t size() const {
    // Not implemented
  }

  void swap(IConteiner& /*a*/) {
    // Not implemented
  }

  std::vector<std::pair<const Key, Value>> values() const {
    // Not implemented
  }

  bool insert(const std::pair<const Key, Value>& /*val*/){
    // Not implemented
  }

  bool insert(std::pair<const Key, Value>&& /*val*/){
    // Not implemented
  }

  bool erase(const Key& /*key*/){
    // Not implemented
  }

  void clear() noexcept {
    // Not implemented
  }

  size_t count(const Key& key) const {
    // Not implemented
  }

  bool find(const Key& key) const {
    // Not implemented
  }

  ~Map() {
    // Not implemented
  }


private:
  /*???*/

}