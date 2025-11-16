#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>

namespace cru {
template <typename T>
class ClonePtr {
  template <typename O>
  friend class ClonePtr;

 public:
  using element_type = typename std::unique_ptr<T>::element_type;
  using pointer = typename std::unique_ptr<T>::pointer;

  ClonePtr() = default;
  ClonePtr(std::nullptr_t) noexcept : ptr_(nullptr) {}
  explicit ClonePtr(pointer p) noexcept : ptr_(p) {}
  ClonePtr(std::unique_ptr<element_type>&& p) noexcept : ptr_(std::move(p)) {}
  template <typename O,
            std::enable_if_t<
                std::is_convertible_v<typename ClonePtr<O>::pointer, pointer>,
                int> = 0>
  ClonePtr(std::unique_ptr<O>&& p) : ptr_(std::move(p)) {}
  ClonePtr(const ClonePtr& other) : ptr_(other.ptr_->Clone()) {}
  ClonePtr(ClonePtr&& other) = default;
  template <typename O,
            std::enable_if_t<
                std::is_convertible_v<typename ClonePtr<O>::pointer, pointer>,
                int> = 0>
  ClonePtr(const ClonePtr<O>& other) : ptr_(other.ptr_->Clone()) {}
  template <typename O,
            std::enable_if_t<
                std::is_convertible_v<typename ClonePtr<O>::pointer, pointer>,
                int> = 0>
  ClonePtr(ClonePtr<O>&& other) noexcept : ptr_(std::move(other.ptr_)) {}
  ClonePtr& operator=(std::nullptr_t) noexcept {
    ptr_ = nullptr;
    return *this;
  }
  ClonePtr& operator=(std::unique_ptr<element_type>&& other) noexcept {
    ptr_ = std::move(other);
    return *this;
  }
  template <typename O,
            std::enable_if_t<
                std::is_convertible_v<typename ClonePtr<O>::pointer, pointer>,
                int> = 0>
  ClonePtr& operator=(std::unique_ptr<O>&& p) noexcept {
    ptr_ = std::move(p);
    return *this;
  }
  ClonePtr& operator=(const ClonePtr& other) {
    if (this != &other) {
      ptr_ = std::unique_ptr<element_type>(other.ptr_->Clone());
    }
    return *this;
  }
  ClonePtr& operator=(ClonePtr&& other) = default;
  template <typename O,
            std::enable_if_t<
                std::is_convertible_v<typename ClonePtr<O>::pointer, pointer>,
                int> = 0>
  ClonePtr& operator=(const ClonePtr<O>& other) noexcept {
    if (this != &other) {
      ptr_ = std::unique_ptr<element_type>(other.ptr_->Clone());
    }
    return *this;
  }
  template <typename O,
            std::enable_if_t<
                std::is_convertible_v<typename ClonePtr<O>::pointer, pointer>,
                int> = 0>
  ClonePtr& operator=(ClonePtr<O>&& other) noexcept {
    ptr_ = std::move(other.ptr_);
  }

  ~ClonePtr() = default;

 public:
  pointer release() noexcept { return ptr_.release(); }
  void reset(pointer p = pointer()) noexcept { ptr_.reset(p); }
  void swap(ClonePtr& other) noexcept { ptr_.swap(other.ptr_); }

 public:
  pointer get() const noexcept { return ptr_.get(); }

  operator bool() const noexcept { return ptr_ != nullptr; }

  element_type& operator*() const noexcept { return *ptr_; }
  pointer operator->() const noexcept { return ptr_.get(); }

  auto operator<=>(std::nullptr_t) const { return ptr_.operator<=>(nullptr); }

  auto operator<=>(const ClonePtr& other) const {
    return ptr_.operator<=>(other.ptr_);
  }

 private:
  std::unique_ptr<element_type> ptr_;
};

template <typename T>
void swap(ClonePtr<T>& left, ClonePtr<T>& right) noexcept {
  left.swap(right);
}

}  // namespace cru

namespace std {
template <typename T>
struct hash<cru::ClonePtr<T>> {
  std::size_t operator()(const cru::ClonePtr<T>& p) const {
    return std::hash<typename cru::ClonePtr<T>::pointer>(p.get());
  }
};
}  // namespace std
