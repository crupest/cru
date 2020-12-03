#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>

namespace cru {
template <typename TClonable>
class ClonablePtr {
 public:
  using element_type = typename std::unique_ptr<TClonable>::element_type;
  using pointer = typename std::unique_ptr<TClonable>::pointer;

  ClonablePtr() = default;
  ClonablePtr(std::nullptr_t) noexcept : ptr_(nullptr) {}
  ClonablePtr(pointer p) noexcept : ptr_(p) {}
  ClonablePtr(std::unique_ptr<element_type>&& p) noexcept
      : ptr_(std::move(p)) {}
  template <typename O,
            std::enable_if_t<std::is_convertible_v<
                                 typename ClonablePtr<O>::pointer, pointer>,
                             int> = 0>
  ClonablePtr(std::unique_ptr<O>&& p) : ptr_(std::move(p)) {}
  ClonablePtr(const ClonablePtr& other) : ptr_(other.ptr_->Clone()) {}
  ClonablePtr(ClonablePtr&& other) = default;
  template <typename O,
            std::enable_if_t<std::is_convertible_v<
                                 typename ClonablePtr<O>::pointer, pointer>,
                             int> = 0>
  ClonablePtr(const ClonablePtr<O>& other) : ptr_(other.ptr_->Clone()) {}
  template <typename O,
            std::enable_if_t<std::is_convertible_v<
                                 typename ClonablePtr<O>::pointer, pointer>,
                             int> = 0>
  ClonablePtr(ClonablePtr<O>&& other) noexcept : ptr_(std::move(other.ptr_)) {}
  ClonablePtr& operator=(std::nullptr_t) noexcept {
    ptr_ = nullptr;
    return *this;
  }
  ClonablePtr& operator=(std::unique_ptr<element_type>&& other) noexcept {
    ptr_ = std::move(other);
    return *this;
  }
  template <typename O,
            std::enable_if_t<std::is_convertible_v<
                                 typename ClonablePtr<O>::pointer, pointer>,
                             int> = 0>
  ClonablePtr& operator=(std::unique_ptr<O>&& p) noexcept {
    ptr_ = std::move(p);
    return *this;
  }
  ClonablePtr& operator=(const ClonablePtr& other) {
    if (this != &other) {
      ptr_ = std::unique_ptr<element_type>(other.ptr->Clone());
    }
    return *this;
  }
  ClonablePtr& operator=(ClonablePtr&& other) = default;
  template <typename O,
            std::enable_if_t<std::is_convertible_v<
                                 typename ClonablePtr<O>::pointer, pointer>,
                             int> = 0>
  ClonablePtr& operator=(const ClonablePtr<O>& other) noexcept {
    if (this != &other) {
      ptr_ = std::unique_ptr<element_type>(other.ptr_->Clone());
    }
    return *this;
  }
  template <typename O,
            std::enable_if_t<std::is_convertible_v<
                                 typename ClonablePtr<O>::pointer, pointer>,
                             int> = 0>
  ClonablePtr& operator=(ClonablePtr<O>&& other) noexcept {
    ptr_ = std::move(other.ptr_);
  }

  ~ClonablePtr() = default;

 public:
  pointer release() noexcept { return ptr_.release(); }
  void reset(pointer p = pointer()) noexcept { ptr_.reset(p); }
  void swap(ClonablePtr& other) noexcept { ptr_.swap(other.ptr_); }

 public:
  pointer get() const noexcept { return ptr_.get(); }

  operator bool() const noexcept { return ptr_; }

  element_type& operator*() const noexcept { return *ptr_; }
  pointer operator->() const noexcept { return ptr_.get(); }

 private:
  std::unique_ptr<element_type> ptr_;
};

template <typename T>
void swap(ClonablePtr<T>& left, ClonablePtr<T>& right) noexcept {
  left.swap(right);
}

template <typename T>
bool operator==(const ClonablePtr<T>& left, const ClonablePtr<T>& right) {
  return left.get() == right.get();
}

template <typename T>
bool operator!=(const ClonablePtr<T>& left, const ClonablePtr<T>& right) {
  return left.get() != right.get();
}

template <typename T>
bool operator<(const ClonablePtr<T>& left, const ClonablePtr<T>& right) {
  return left.get() < right.get();
}

template <typename T>
bool operator<=(const ClonablePtr<T>& left, const ClonablePtr<T>& right) {
  return left.get() <= right.get();
}

template <typename T>
bool operator>(const ClonablePtr<T>& left, const ClonablePtr<T>& right) {
  return left.get() > right.get();
}

template <typename T>
bool operator>=(const ClonablePtr<T>& left, const ClonablePtr<T>& right) {
  return left.get() >= right.get();
}

template <typename T>
bool operator==(const ClonablePtr<T>& left, std::nullptr_t) {
  return left.get() == nullptr;
}

template <typename T>
bool operator!=(const ClonablePtr<T>& left, std::nullptr_t) {
  return left.get() != nullptr;
}

template <typename T>
bool operator<(const ClonablePtr<T>& left, std::nullptr_t) {
  return left.get() < nullptr;
}

template <typename T>
bool operator<=(const ClonablePtr<T>& left, std::nullptr_t) {
  return left.get() <= nullptr;
}

template <typename T>
bool operator>(const ClonablePtr<T>& left, std::nullptr_t) {
  return left.get() > nullptr;
}

template <typename T>
bool operator>=(const ClonablePtr<T>& left, std::nullptr_t) {
  return left.get() >= nullptr;
}

template <typename T>
bool operator==(std::nullptr_t, const ClonablePtr<T>& right) {
  return nullptr == right.get();
}

template <typename T>
bool operator!=(std::nullptr_t, const ClonablePtr<T>& right) {
  return nullptr != right.get();
}

template <typename T>
bool operator<(std::nullptr_t, const ClonablePtr<T>& right) {
  return nullptr < right.get();
}

template <typename T>
bool operator<=(std::nullptr_t, const ClonablePtr<T>& right) {
  return nullptr <= right.get();
}

template <typename T>
bool operator>(std::nullptr_t, const ClonablePtr<T>& right) {
  return nullptr > right.get();
}

template <typename T>
bool operator>=(std::nullptr_t, const ClonablePtr<T>& right) {
  return nullptr >= right.get();
}

}  // namespace cru

namespace std {
template <typename T>
struct hash<cru::ClonablePtr<T>> {
  std::size_t operator()(const cru::ClonablePtr<T>& p) const {
    return std::hash<typename cru::ClonablePtr<T>::pointer>(p.get());
  }
};
}  // namespace std
