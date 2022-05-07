#pragma once
#include "Base.h"

#include <memory>
#include <utility>

namespace cru::ui {
class CRU_UI_API DeleteLaterImpl {
 public:
  virtual ~DeleteLaterImpl();
  void DeleteLater();

 protected:
  virtual void OnPrepareDelete();
};

template <typename T>
class DeleteLaterPtr {
 public:
  DeleteLaterPtr() = default;

  DeleteLaterPtr(const DeleteLaterPtr& other) = delete;
  DeleteLaterPtr& operator=(const DeleteLaterPtr& other) = delete;

  DeleteLaterPtr(DeleteLaterPtr&& other) noexcept : ptr_(other.ptr_) {
    other.ptr_ = nullptr;
  }

  DeleteLaterPtr& operator=(DeleteLaterPtr&& other) noexcept {
    if (this != &other) {
      if (ptr_ != nullptr) {
        ptr_->DeleteLater();
      }

      ptr_ = other.ptr_;
      other.ptr_ = nullptr;
    }
    return *this;
  }

  ~DeleteLaterPtr() {
    if (ptr_ != nullptr) {
      ptr_->DeleteLater();
    }
  }

  explicit DeleteLaterPtr(T* ptr) : ptr_(ptr) {}
  DeleteLaterPtr(std::unique_ptr<T>&& ptr) : ptr_(ptr.release()) {}

  T& operator*() const { return *ptr_; }
  T* operator->() const { return ptr_; }

  explicit operator bool() const { return ptr_ != nullptr; }

  T* get() const { return ptr_; }

 private:
  T* ptr_ = nullptr;
};

template <typename T, typename... Args>
DeleteLaterPtr<T> MakeDeleteLaterPtr(Args&&... args) {
  return DeleteLaterPtr<T>(new T(std::forward<Args>(args)...));
}
}  // namespace cru::ui
