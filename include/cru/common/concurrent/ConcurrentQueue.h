#pragma once
#include <condition_variable>
#include <mutex>
#include <optional>
#include <utility>

namespace cru::concurrent {
namespace details {
template <typename T>
struct ConcurrentQueueNode {
  ConcurrentQueueNode(T&& value, ConcurrentQueueNode* next = nullptr)
      : value(std::move(value)), next(next) {}

  T value;
  ConcurrentQueueNode* next;
};
}  // namespace details

template <typename T>
class ConcurrentQueue {
 public:
  ConcurrentQueue() {}

  ConcurrentQueue(const ConcurrentQueue&) = delete;
  ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

  ConcurrentQueue(ConcurrentQueue&& other)
      : head_(other.head_),
        tail_(other.tail_),
        mutex_(std::move(other.mutex_)),
        condition_variable_(std::move(other.condition_variable_)) {
    other.head_ = nullptr;
    other.tail_ = nullptr;
  }

  ConcurrentQueue& operator=(ConcurrentQueue&& other) {
    if (this != &other) {
      head_ = other.head_;
      tail_ = other.tail_;
      mutex_ = std::move(other.mutex_);
      condition_variable_ = std::move(other.condition_variable_);
      other.head_ = nullptr;
      other.tail_ = nullptr;
      return *this;
    }
    return *this;
  }

  ~ConcurrentQueue() {
    if (head_) {
      auto node = head_;
      while (node) {
        auto next = node->next;
        delete node;
        node = next;
      }
    }
  }

 public:
  void Push(T&& value) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (head_ == nullptr) {
      head_ = tail_ = new details::ConcurrentQueueNode<T>(std::move(value));
      condition_variable_.notify_one();
    } else {
      tail_->next = new details::ConcurrentQueueNode<T>(std::move(value));
      tail_ = tail_->next;
    }
  }

  T Pull() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (head_ == nullptr) {
      condition_variable_.wait(lock);
    }
    assert(head_ != nullptr);
    auto value = std::move(head_->value);
    auto next = head_->next;
    delete head_;
    head_ = next;
    if (next == nullptr) {
      tail_ = nullptr;
    }
    return value;
  }

  std::optional<T> Poll() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (head_ == nullptr) {
      return std::nullopt;
    }
    auto value = std::move(head_->value);
    auto next = head_->next;
    delete head_;
    head_ = next;
    if (next == nullptr) {
      tail_ = nullptr;
    }
    return value;
  }

 private:
  details::ConcurrentQueueNode<T>* head_ = nullptr;
  details::ConcurrentQueueNode<T>* tail_ = nullptr;

  std::mutex mutex_;
  std::condition_variable condition_variable_;
};
}  // namespace cru::concurrent
