#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>

namespace cru {
template <typename TClonable>
class CopyPtr {
  template <typename T>
  friend class CopyPtr;

 public:
  using element_type = typename std::unique_ptr<TClonable>::element_type;
  using pointer = typename std::unique_ptr<TClonable>::pointer;

  CopyPtr() = default;
  CopyPtr(std::nullptr_t) noexcept : ptr_(nullptr) {}
  explicit CopyPtr(pointer p) noexcept : ptr_(p) {}
  CopyPtr(std::unique_ptr<element_type>&& p) noexcept : ptr_(std::move(p)) {}
  template <typename O,
            std::enable_if_t<
                std::is_convertible_v<typename CopyPtr<O>::pointer, pointer>,
                int> = 0>
  CopyPtr(std::unique_ptr<O>&& p) : ptr_(std::move(p)) {}
  CopyPtr(const CopyPtr& other) : ptr_(other.ptr_->Clone()) {}
  CopyPtr(CopyPtr&& other) = default;
  template <typename O,
            std::enable_if_t<
                std::is_convertible_v<typename CopyPtr<O>::pointer, pointer>,
                int> = 0>
  CopyPtr(const CopyPtr<O>& other) : ptr_(other.ptr_->Clone()) {}
  template <typename O,
            std::enable_if_t<
                std::is_convertible_v<typename CopyPtr<O>::pointer, pointer>,
                int> = 0>
  CopyPtr(CopyPtr<O>&& other) noexcept : ptr_(std::move(other.ptr_)) {}
  CopyPtr& operator=(std::nullptr_t) noexcept {
    ptr_ = nullptr;
    return *this;
  }
  CopyPtr& operator=(std::unique_ptr<element_type>&& other) noexcept {
    ptr_ = std::move(other);
    return *this;
  }
  template <typename O,
            std::enable_if_t<
                std::is_convertible_v<typename CopyPtr<O>::pointer, pointer>,
                int> = 0>
  CopyPtr& operator=(std::unique_ptr<O>&& p) noexcept {
    ptr_ = std::move(p);
    return *this;
  }
  CopyPtr& operator=(const CopyPtr& other) {
    if (this != &other) {
      ptr_ = std::unique_ptr<element_type>(other.ptr_->Clone());
    }
    return *this;
  }
  CopyPtr& operator=(CopyPtr&& other) = default;
  template <typename O,
            std::enable_if_t<
                std::is_convertible_v<typename CopyPtr<O>::pointer, pointer>,
                int> = 0>
  CopyPtr& operator=(const CopyPtr<O>& other) noexcept {
    if (this != &other) {
      ptr_ = std::unique_ptr<element_type>(other.ptr_->Clone());
    }
    return *this;
  }
  template <typename O,
            std::enable_if_t<
                std::is_convertible_v<typename CopyPtr<O>::pointer, pointer>,
                int> = 0>
  CopyPtr& operator=(CopyPtr<O>&& other) noexcept {
    ptr_ = std::move(other.ptr_);
  }

  ~CopyPtr() = default;

 public:
  pointer release() noexcept { return ptr_.release(); }
  void reset(pointer p = pointer()) noexcept { ptr_.reset(p); }
  void swap(CopyPtr& other) noexcept { ptr_.swap(other.ptr_); }

 public:
  pointer get() const noexcept { return ptr_.get(); }

  operator bool() const noexcept { return ptr_ != nullptr; }

  element_type& operator*() const noexcept { return *ptr_; }
  pointer operator->() const noexcept { return ptr_.get(); }

  int Compare(const CopyPtr& other) const noexcept {
    if (ptr_ == other.ptr_) {
      return 0;
    } else if (ptr_ < other.ptr_) {
      return -1;
    } else {
      return 1;
    }
  }

  int Compare(nullptr_t) const noexcept { return ptr_ ? 1 : 0; }

 private:
  std::unique_ptr<element_type> ptr_;
};

template <typename T>
void swap(CopyPtr<T>& left, CopyPtr<T>& right) noexcept {
  left.swap(right);
}

template <typename T>
bool operator==(const CopyPtr<T>& left, const CopyPtr<T>& right) {
  return left.Compare(right) == 0;
}

template <typename T>
bool operator!=(const CopyPtr<T>& left, const CopyPtr<T>& right) {
  return left.Compare(right) != 0;
}

template <typename T>
bool operator<(const CopyPtr<T>& left, const CopyPtr<T>& right) {
  return left.Compare(right) < 0;
}

template <typename T>
bool operator<=(const CopyPtr<T>& left, const CopyPtr<T>& right) {
  return left.Compare(right) <= 0;
}

template <typename T>
bool operator>(const CopyPtr<T>& left, const CopyPtr<T>& right) {
  return left.Compare(right) > 0;
}

template <typename T>
bool operator>=(const CopyPtr<T>& left, const CopyPtr<T>& right) {
  return left.Compare(right) >= 0;
}

template <typename T>
bool operator==(const CopyPtr<T>& left, std::nullptr_t) {
  return left.Compare(nullptr) == 0;
}

template <typename T>
bool operator!=(const CopyPtr<T>& left, std::nullptr_t) {
  return left.Compare(nullptr) != 0;
}

template <typename T>
bool operator<(const CopyPtr<T>& left, std::nullptr_t) {
  return left.Compare(nullptr) < 0;
}

template <typename T>
bool operator<=(const CopyPtr<T>& left, std::nullptr_t) {
  return left.Compare(nullptr) <= 0;
}

template <typename T>
bool operator>(const CopyPtr<T>& left, std::nullptr_t) {
  return left.Compare(nullptr) > 0;
}

template <typename T>
bool operator>=(const CopyPtr<T>& left, std::nullptr_t) {
  return left.Compare(nullptr) >= 0;
}

template <typename T>
bool operator==(std::nullptr_t, const CopyPtr<T>& right) {
  return right.Compare(nullptr) == 0;
}

template <typename T>
bool operator!=(std::nullptr_t, const CopyPtr<T>& right) {
  return right.Compare(nullptr) != 0;
}

template <typename T>
bool operator<(std::nullptr_t, const CopyPtr<T>& right) {
  return right.Compare(nullptr) > 0;
}

template <typename T>
bool operator<=(std::nullptr_t, const CopyPtr<T>& right) {
  return right.Compare(nullptr) >= 0;
}

template <typename T>
bool operator>(std::nullptr_t, const CopyPtr<T>& right) {
  return right.Compare(nullptr) < 0;
}

template <typename T>
bool operator>=(std::nullptr_t, const CopyPtr<T>& right) {
  return right.Compare(nullptr) <= 0;
}

}  // namespace cru

namespace std {
template <typename T>
struct hash<cru::CopyPtr<T>> {
  std::size_t operator()(const cru::CopyPtr<T>& p) const {
    return std::hash<typename cru::CopyPtr<T>::pointer>(p.get());
  }
};
}  // namespace std
