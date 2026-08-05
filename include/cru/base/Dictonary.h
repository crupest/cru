#pragma once

#include "Base.h"

#include <functional>
#include <list>
#include <stdexcept>
#include <utility>

namespace cru {
enum class DictionaryInsertResult { None, Inserted, Overwritten };

template <typename Key, typename T, typename KeyEqual = std::equal_to<Key>,
          typename Allocator = std::allocator<std::pair<const Key, T>>>
class Dictionary {
 public:
  using Storage = std::list<std::pair<const Key, T>, Allocator>;
  using key_type = Key;
  using mapped_type = T;
  using key_equal = KeyEqual;
  using value_type = Storage::value_type;
  using size_type = Storage::size_type;
  using difference_type = Storage::difference_type;
  using allocator_type = Storage::allocator_type;
  using reference = Storage::reference;
  using const_reference = Storage::const_reference;
  using pointer = Storage::pointer;
  using const_pointer = Storage::const_pointer;
  using iterator = Storage::iterator;
  using const_iterator = Storage::const_iterator;
  using reverse_iterator = Storage::reverse_iterator;
  using const_reverse_iterator = Storage::const_reverse_iterator;

  CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(Dictionary)
  CRU_DEFAULT_COPY(Dictionary)
  CRU_DEFAULT_MOVE(Dictionary)

  constexpr auto begin() { return entries_.begin(); }
  constexpr auto begin() const { return entries_.begin(); }
  constexpr auto cbegin() const { return entries_.cbegin(); }
  constexpr auto end() { return entries_.end(); }
  constexpr auto end() const { return entries_.end(); }
  constexpr auto cend() const { return entries_.cend(); }
  constexpr auto rbegin() { return entries_.rbegin(); }
  constexpr auto rbegin() const { return entries_.rbegin(); }
  constexpr auto crbegin() const { return entries_.crbegin(); }
  constexpr auto rend() { return entries_.rend(); }
  constexpr auto rend() const { return entries_.rend(); }
  constexpr auto crend() const { return entries_.crend(); }

  constexpr auto empty() const { return entries_.empty(); }
  constexpr auto size() const { return entries_.size(); }
  constexpr auto max_size() const { return entries_.max_size(); }

  template <typename K>
  iterator find(const K& key) {
    auto end = entries_.end();
    for (auto iter = entries_.begin(); iter != end; ++iter) {
      if (key_equal_(iter->first, key)) {
        return iter;
      }
    }
    return end;
  }

  template <typename K>
  const_iterator find(const K& key) const {
    auto end = entries_.end();
    for (auto iter = entries_.begin(); iter != end; ++iter) {
      if (key_equal_(iter->first, key)) {
        return iter;
      }
    }
    return end;
  }

  template <typename K>
  T& at(const K& key) {
    auto iter = find(key);
    if (iter == end()) {
      throw std::out_of_range("Dictionary::at: key not found");
    }
    return iter->second;
  }

  template <typename K>
  const T& at(const K& key) const {
    auto iter = find(key);
    if (iter == end()) {
      throw std::out_of_range("Dictionary::at: key not found");
    }
    return iter->second;
  }

  template <typename K>
  T& operator[](K&& key) {
    auto iter = find(key);
    if (iter == end()) {
      entries_.emplace_back(std::forward<K>(key), T{});
      return entries_.back().second;
    }
    return iter->second;
  }

  template <typename... Args>
  DictionaryInsertResult TryEmplace(const key_type& key, Args&&... args) {
    auto iter = find(key);
    if (iter == end()) {
      entries_.emplace_back(key, T(std::forward<Args>(args)...));
      return DictionaryInsertResult::Inserted;
    } else {
      return DictionaryInsertResult::None;
    }
  }

  template <typename... Args>
  DictionaryInsertResult TryEmplace(key_type&& key, Args&&... args) {
    auto iter = find(key);
    if (iter == end()) {
      entries_.emplace_back(std::move(key), T(std::forward<Args>(args)...));
      return DictionaryInsertResult::Inserted;
    } else {
      return DictionaryInsertResult::None;
    }
  }

  template <typename... Args>
  DictionaryInsertResult ForceEmplace(const key_type& key, Args&&... args) {
    auto iter = find(key);
    if (iter == end()) {
      entries_.emplace_back(key, T(std::forward<Args>(args)...));
      return DictionaryInsertResult::Inserted;
    } else {
      iter->second = T(std::forward<Args>(args)...);
      return DictionaryInsertResult::Overwritten;
    }
  }

  template <typename... Args>
  DictionaryInsertResult ForceEmplace(key_type&& key, Args&&... args) {
    auto iter = find(key);
    if (iter == end()) {
      entries_.emplace_back(std::move(key), T(std::forward<Args>(args)...));
      return DictionaryInsertResult::Inserted;
    } else {
      iter->second = T(std::forward<Args>(args)...);
      return DictionaryInsertResult::Overwritten;
    }
  }

  template <typename K>
  bool Remove(const K& key) {
    auto iter = find(key);
    if (iter == end()) {
      return false;
    }
    entries_.erase(iter);
    return true;
  }

 private:
  Storage entries_;
  KeyEqual key_equal_;
};
}  // namespace cru
