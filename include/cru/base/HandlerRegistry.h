#pragma once
#include "Base.h"

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

namespace cru {

template <typename T>
class HandlerRegistryIterator {
 public:
  using RawIterator =
      typename std::vector<std::pair<int, std::function<T>>>::const_iterator;

  explicit HandlerRegistryIterator(RawIterator raw) : raw_(std::move(raw)) {}

  CRU_DELETE_COPY(HandlerRegistryIterator)
  CRU_DELETE_MOVE(HandlerRegistryIterator)

  ~HandlerRegistryIterator() = default;

  const std::function<T>& operator*() const { return raw_->second; }
  const std::function<T>* operator->() const { return &raw_->second; }

  HandlerRegistryIterator& operator++() {
    ++raw_;
    return *this;
  }

  HandlerRegistryIterator operator++(int) {
    auto c = *this;
    this->operator++();
    return c;
  }

  bool operator==(const HandlerRegistryIterator<T>& other) const {
    return this->raw_ == other.raw_;
  }

  bool operator!=(const HandlerRegistryIterator<T>& other) const {
    return !this->operator==(other);
  }

 private:
  RawIterator raw_;
};

template <typename T>
class HandlerRegistry final {
 public:
  HandlerRegistry() = default;
  CRU_DEFAULT_COPY(HandlerRegistry)
  CRU_DEFAULT_MOVE(HandlerRegistry)
  ~HandlerRegistry() = default;

 public:
  int AddHandler(std::function<T> handler) {
    auto id = current_id_++;
    handler_list_.push_back({id, std::move(handler)});
    return id;
  }

  void RemoveHandler(int id) {
    auto result = std::find_if(handler_list_.cbegin(), handler_list_.cend(),
                               [id](const std::pair<int, std::function<T>>& d) {
                                 return d.first == id;
                               });
    if (result != handler_list_.cend()) {
      handler_list_.erase(result);
    }
  }

  HandlerRegistryIterator<T> begin() const {
    return HandlerRegistryIterator<T>(handler_list_.begin());
  }

  HandlerRegistryIterator<T> end() const {
    return HandlerRegistryIterator<T>(handler_list_.end());
  }

 private:
  int current_id_ = 1;
  std::vector<std::pair<int, std::function<T>>> handler_list_;
};
}  // namespace cru
