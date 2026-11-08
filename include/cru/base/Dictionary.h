#pragma once

#include "Base.h"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace cru {
/**
 * @brief Insertion-ordered associative container.
 *
 * The value and iterator model mirrors std::flat_map:
 *
 * - value_type is std::pair<Key, T>; the key is not const.
 * - reference and const_reference are proxy pairs
 *   (std::pair<const Key&, T&>) returned by value, so `*it` is a prvalue.
 *   Iterate with `auto&&`, structured bindings, or `const auto&`;
 *   `for (auto& item : dictionary)` does not compile.
 * - Iterators meet the Cpp17InputIterator requirements and model
 *   random_access_iterator. `--`, `+`, `[]` and ranges algorithms work, while
 *   std::sort and std::reverse are rejected. Prefer std::ranges::prev/next over
 *   std::prev/std::next with a negative offset.
 * - Elements are stored contiguously. Any insertion invalidates iterators,
 *   references and pointers to elements; erasing shifts later elements.
 */
template <typename Key, typename T>
class Dictionary {
 public:
  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<Key, T>;
  using size_type = std::size_t;
  using difference_type = Index;
  using reference = std::pair<const key_type&, mapped_type&>;
  using const_reference = std::pair<const key_type&, const mapped_type&>;

 private:
  // Dictionary accepts heterogeneous key types for every key-taking operation.
  // Only one direction is checked because the members always evaluate
  // `key == candidate`, never the reverse. This is a trait rather than a
  // concept because a concept can only be declared at namespace scope.
  // clang-format off
  // (clang-format mis-parses a requires-expression used as a template argument)
  template <typename K>
  struct KeyComparable
      : std::bool_constant<requires(const Key& key, const K& candidate) {
          { key == candidate } -> std::convertible_to<bool>;
        }> {};
  // clang-format on

  using Storage = std::vector<value_type>;

 public:
  template <bool IsConst>
  class Iterator {
   private:
    using StorageIterator =
        std::conditional_t<IsConst, typename Storage::const_iterator,
                           typename Storage::iterator>;

    class PointerProxy;

   public:
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = std::random_access_iterator_tag;
    using value_type = typename Dictionary::value_type;
    using difference_type = typename Dictionary::difference_type;
    using reference =
        std::conditional_t<IsConst, typename Dictionary::const_reference,
                           typename Dictionary::reference>;
    using pointer = PointerProxy;

   private:
    class PointerProxy {
     public:
      PointerProxy(const reference& ref) : ref_(ref) {}
      const reference* operator->() const { return &ref_; }

     private:
      reference ref_;
    };

   public:
    Iterator() = default;

    template <bool OtherIsConst>
      requires(IsConst && !OtherIsConst)
    Iterator(const Iterator<OtherIsConst>& other) : base_(other.base_) {}

    reference operator*() const {
      return reference(base_->first, base_->second);
    }

    PointerProxy operator->() const { return PointerProxy(**this); }

    reference operator[](difference_type offset) const {
      return *(*this + offset);
    }

    Iterator& operator++() { return *this += 1; }

    Iterator operator++(int) {
      Iterator copy(*this);
      ++*this;
      return copy;
    }

    Iterator& operator--() { return *this -= 1; }

    Iterator operator--(int) {
      Iterator copy(*this);
      --*this;
      return copy;
    }

    Iterator& operator+=(difference_type offset) {
      base_ += offset;
      return *this;
    }

    Iterator& operator-=(difference_type offset) {
      base_ -= offset;
      return *this;
    }

    friend Iterator operator+(Iterator iter, difference_type offset) {
      iter += offset;
      return iter;
    }

    friend Iterator operator+(difference_type offset, Iterator iter) {
      iter += offset;
      return iter;
    }

    friend Iterator operator-(Iterator iter, difference_type offset) {
      iter -= offset;
      return iter;
    }

    friend difference_type operator-(const Iterator& lhs, const Iterator& rhs) {
      return lhs.base_ - rhs.base_;
    }

    friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
      return operator<=>(lhs, rhs) == 0;
    }

    friend auto operator<=>(const Iterator& lhs, const Iterator& rhs) {
      return lhs.base_ <=> rhs.base_;
    }

   private:
    friend class Dictionary;

    template <bool>
    friend class Iterator;

    explicit Iterator(StorageIterator base) : base_(base) {}

    StorageIterator base_;
  };

  using iterator = Iterator<false>;
  using const_iterator = Iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  Dictionary() = default;

  Dictionary(std::initializer_list<value_type> init) : storage_(init) {}

  auto begin() { return iterator(storage_.begin()); }
  auto end() { return iterator(storage_.end()); }
  auto begin() const { return const_iterator(storage_.begin()); }
  auto end() const { return const_iterator(storage_.end()); }
  auto cbegin() const { return const_iterator(storage_.cbegin()); }
  auto cend() const { return const_iterator(storage_.cend()); }
  auto rbegin() { return reverse_iterator(end()); }
  auto rend() { return reverse_iterator(begin()); }
  auto rbegin() const { return const_reverse_iterator(end()); }
  auto rend() const { return const_reverse_iterator(begin()); }
  auto crbegin() const { return const_reverse_iterator(cend()); }
  auto crend() const { return const_reverse_iterator(cbegin()); }

  auto empty() const noexcept { return storage_.empty(); }
  auto size() const noexcept { return storage_.size(); }
  Index ssize() const noexcept { return static_cast<Index>(size()); }
  auto max_size() const noexcept { return storage_.max_size(); }

  auto clear() noexcept { return storage_.clear(); }

  template <typename P>
  std::pair<iterator, bool> insert(P&& value) {
    return emplace(std::forward<P>(value));
  }

  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    static_assert(std::is_constructible_v<value_type, Args&&...>);
    value_type new_value(std::forward<Args>(args)...);
    auto it = std::ranges::find_if(storage_, [&new_value](const auto& pair) {
      return pair.first == new_value.first;
    });
    if (it != storage_.end()) {
      return {iterator(it), false};
    } else {
      storage_.push_back(std::move(new_value));
      return {std::prev(end()), true};
    }
  }

  template <class K, class M>
  std::pair<iterator, bool> insert_or_assign(K&& k, M&& obj) {
    static_assert(KeyComparable<K>::value);
    static_assert(std::is_constructible_v<Key, K&&>);
    static_assert(std::is_constructible_v<T, M&&> &&
                  std::is_assignable_v<T&, M&&>);
    auto it = std::ranges::find_if(
        storage_, [&k](const auto& pair) { return pair.first == k; });
    if (it != storage_.end()) {
      it->second = std::forward<M>(obj);
      return {iterator(it), false};
    } else {
      storage_.emplace_back(std::forward<K>(k), std::forward<M>(obj));
      return {std::prev(end()), true};
    }
  }

  auto erase(const_iterator pos) { return iterator(storage_.erase(pos.base_)); }
  auto erase(iterator pos) { return iterator(storage_.erase(pos.base_)); }
  auto erase(const_iterator first, const_iterator last) {
    return iterator(storage_.erase(first.base_, last.base_));
  }
  template <typename K>
  size_type erase(K&& key) {
    static_assert(KeyComparable<K>::value);
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
    static_assert(KeyComparable<K>::value);
    auto it = std::ranges::find_if(
        storage_, [&key](const auto& pair) { return pair.first == key; });
    if (it != storage_.end()) {
      return it->second;
    }
    throw std::out_of_range("Key not found in Dictionary");
  }

  template <typename K>
  const T& at(const K& key) const {
    static_assert(KeyComparable<K>::value);
    auto it = std::ranges::find_if(
        storage_, [&key](const auto& pair) { return pair.first == key; });
    if (it != storage_.end()) {
      return it->second;
    }
    throw std::out_of_range("Key not found in Dictionary");
  }

  template <typename K>
  T& operator[](K&& key) {
    static_assert(KeyComparable<K>::value);
    static_assert(std::is_constructible_v<Key, K&&>);
    static_assert(std::is_default_constructible_v<T>);
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
    static_assert(KeyComparable<K>::value);
    auto it = std::ranges::find_if(
        storage_, [&x](const auto& pair) { return pair.first == x; });
    return iterator(it);
  }

  template <class K>
  const_iterator find(const K& x) const {
    static_assert(KeyComparable<K>::value);
    auto it = std::ranges::find_if(
        storage_, [&x](const auto& pair) { return pair.first == x; });
    return const_iterator(it);
  }

  template <typename K>
  size_type count(const K& key) const {
    static_assert(KeyComparable<K>::value);
    auto it = std::ranges::find_if(
        storage_, [&key](const auto& pair) { return pair.first == key; });
    return it != storage_.end() ? 1 : 0;
  }

  template <typename K>
  bool contains(const K& key) const {
    return count(key) > 0;
  }

  auto Keys() const {
    return std::views::transform(
        storage_, [](const auto& pair) -> const Key& { return pair.first; });
  }

  auto Values() {
    return std::views::transform(storage_,
                                 [](auto& pair) -> T& { return pair.second; });
  }

  auto Values() const {
    return std::views::transform(
        storage_, [](const auto& pair) -> const T& { return pair.second; });
  }

  bool operator==(const Dictionary& other) const {
    if (size() != other.size()) return false;

    auto iter1 = begin();
    auto iter2 = other.begin();

    for (; iter1 != end(); ++iter1, ++iter2) {
      if (*iter1 != *iter2) return false;
    }
    return true;
  }

 private:
  Storage storage_;
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
