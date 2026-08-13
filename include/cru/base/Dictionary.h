#pragma once

#include <algorithm>
#include <iterator>
#include <list>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace cru {
template <typename Key, typename T>
class Dictionary {
 public:
  using StorageType = std::list<std::pair<const Key, T>>;
  using key_type = Key;
  using mapped_type = T;
  using value_type = StorageType::value_type;
  using size_type = StorageType::size_type;
  using difference_type = StorageType::difference_type;
  using allocator_type = StorageType::allocator_type;
  using reference = StorageType::reference;
  using const_reference = StorageType::const_reference;
  using pointer = StorageType::pointer;
  using const_pointer = StorageType::const_pointer;
  using iterator = StorageType::iterator;
  using const_iterator = StorageType::const_iterator;
  using reverse_iterator = StorageType::reverse_iterator;
  using const_reverse_iterator = StorageType::const_reverse_iterator;

  auto begin() { return storage_.begin(); }
  auto end() { return storage_.end(); }
  auto begin() const { return storage_.begin(); }
  auto end() const { return storage_.end(); }
  auto cbegin() const { return storage_.cbegin(); }
  auto cend() const { return storage_.cend(); }
  auto rbegin() { return storage_.rbegin(); }
  auto rend() { return storage_.rend(); }
  auto rbegin() const { return storage_.rbegin(); }
  auto rend() const { return storage_.rend(); }
  auto crbegin() const { return storage_.crbegin(); }
  auto crend() const { return storage_.crend(); }

  auto empty() const noexcept { return storage_.empty(); }
  auto size() const noexcept { return storage_.size(); }
  auto max_size() const noexcept { return storage_.max_size(); }

  auto clear() noexcept { return storage_.clear(); }

  template <typename P>
    requires(std::is_constructible_v<value_type, P &&>)
  std::pair<iterator, bool> insert(P&& value) {
    return emplace(std::forward<P>(value));
  }

  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    value_type new_value(std::forward<Args>(args)...);
    auto it = std::ranges::find_if(storage_, [&new_value](const auto& pair) {
      return pair.first == new_value.first;
    });
    if (it != storage_.end()) {
      return {it, false};
    } else {
      storage_.push_back(std::move(new_value));
      return {std::prev(storage_.end()), true};
    }
  }

  template <typename... Args>
  std::pair<iterator, bool> insert(Args&&... args) {
    return emplace(std::forward<Args>(args)...);
  }

  template <class K, class M>
  std::pair<iterator, bool> insert_or_assign(K&& k, M&& obj) {
    auto it = std::ranges::find_if(
        storage_, [&k](const auto& pair) { return pair.first == k; });
    if (it != storage_.end()) {
      it->second = std::forward<M>(obj);
      return {it, false};
    } else {
      storage_.push_back(value_type(std::forward<K>(k), std::forward<M>(obj)));
      return {std::prev(storage_.end()), true};
    }
  }

  auto erase(const_iterator pos) { return storage_.erase(pos); }
  auto erase(iterator pos) { return storage_.erase(pos); }
  auto erase(const_iterator first, const_iterator last) {
    return storage_.erase(first, last);
  }
  template <typename K>
  size_type erase(K&& key) {
    auto it = std::ranges::find_if(
        storage_, [&key](const auto& pair) { return pair.first == key; });
    if (it != storage_.end()) {
      storage_.erase(it);
      return 1;
    }
    return 0;
  }

  void swap(Dictionary& other) { storage_.swap(other.storage_); }

  template <typename K>
  T& at(const K& key) {
    auto it = std::ranges::find_if(
        storage_, [&key](const auto& pair) { return pair.first == key; });
    if (it != storage_.end()) {
      return it->second;
    }
    throw std::out_of_range("Key not found in Dictionary");
  }

  template <typename K>
  const T& at(const K& key) const {
    auto it = std::ranges::find_if(
        storage_, [&key](const auto& pair) { return pair.first == key; });
    if (it != storage_.end()) {
      return it->second;
    }
    throw std::out_of_range("Key not found in Dictionary");
  }

  template <typename K>
  T& operator[](K&& key) {
    auto it = std::ranges::find_if(
        storage_, [&key](const auto& pair) { return pair.first == key; });
    if (it != storage_.end()) {
      return it->second;
    } else {
      storage_.emplace_back(std::forward<K>(key), T{});
      return storage_.back().second;
    }
  }

  template <class K>
  iterator find(const K& x) {
    auto it = std::ranges::find_if(
        storage_, [&x](const auto& pair) { return pair.first == x; });
    return it;
  }

  template <class K>
  const_iterator find(const K& x) const {
    auto it = std::ranges::find_if(
        storage_, [&x](const auto& pair) { return pair.first == x; });
    return it;
  }

  template <typename K>
  size_type count(const K& key) const {
    auto it = std::ranges::find_if(
        storage_, [&key](const auto& pair) { return pair.first == key; });
    return it != storage_.end() ? 1 : 0;
  }

  template <typename K>
  bool contains(const K& key) const {
    return count(key) > 0;
  }

  bool operator==(const Dictionary& other) const {
    return storage_ == other.storage_;
  }

 private:
  StorageType storage_;
};

template <typename Key, typename T>
void swap(Dictionary<Key, T>& lhs, Dictionary<Key, T>& rhs) {
  lhs.swap(rhs);
}

template <class Key, class T, class Pred>
typename Dictionary<Key, T>::size_type erase_if(Dictionary<Key, T>& c,
                                                Pred pred) {
  auto old_size = c.size();
  for (auto first = c.begin(); first != c.end();) {
    if (pred(*first))
      first = c.erase(first);
    else
      ++first;
  }
  return old_size - c.size();
}
}  // namespace cru
