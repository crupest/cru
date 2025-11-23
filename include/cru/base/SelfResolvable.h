#pragma once
#include "Base.h"

#include <cassert>
#include <memory>
#include <type_traits>

namespace cru {
template <typename T>
class SelfResolvable;

template <typename T>
class ObjectResolver {
  friend SelfResolvable<T>;
  template <typename U>
  friend class ObjectResolver;

 private:
  explicit ObjectResolver(std::shared_ptr<bool> destroyed, T* ptr)
      : destroyed_(std::move(destroyed)), ptr_(ptr) {}

 public:
  CRU_DEFAULT_COPY(ObjectResolver)
  CRU_DEFAULT_MOVE(ObjectResolver)

  template <typename U>
  ObjectResolver(const ObjectResolver<U>& other)
    requires(std::is_convertible_v<U*, T*>)
      : destroyed_(other.destroyed_), ptr_(static_cast<T*>(other.ptr_)) {}

  template <typename U>
  ObjectResolver(ObjectResolver<U>&& other)
    requires(std::is_convertible_v<U*, T*>)
      : destroyed_(std::move(other.destroyed_)),
        ptr_(static_cast<T*>(other.ptr_)) {}

  template <typename U>
  ObjectResolver& operator=(const ObjectResolver<U>& other)
    requires(std::is_convertible_v<U*, T*>)
  {
    if (this != &other) {
      this->destroyed_ = other.destroyed_;
      this->ptr_ = static_cast<T*>(other.ptr_);
    }
    return *this;
  }

  template <typename U>
  ObjectResolver& operator=(ObjectResolver<U>&& other)
    requires(std::is_convertible_v<U*, T*>)
  {
    if (this != &other) {
      this->destroyed_ = std::move(other.destroyed_);
      this->ptr_ = static_cast<T*>(other.ptr_);
    }
    return *this;
  }

  bool IsValid() const { return this->destroyed_ != nullptr; }

  T* Resolve() const {
    assert(IsValid());
    return *destroyed_ ? nullptr : ptr_;
  }

  /**
   * @remarks So this class can be used as a functor.
   */
  T* operator()() const { return Resolve(); }

  template <typename U>
  operator ObjectResolver<U>() const
    requires(std::is_convertible_v<T*, U*>)
  {
    return ObjectResolver<U>(*this);
  }

 private:
  std::shared_ptr<bool> destroyed_;
  T* ptr_;
};

/**
 * @remarks
 * This class is not copyable and movable since subclass is polymorphic and
 * copying is then nonsense. However, you can even delete move capability in
 * subclass because it may also be nonsense for subclass. The move capability is
 * optional.
 *
 * Whether this class needs to be thread-safe still has to be considered.
 */
template <typename T>
class SelfResolvable {
 public:
  SelfResolvable() : destroyed_(new bool(false)) {}
  CRU_DELETE_COPY(SelfResolvable)
  virtual ~SelfResolvable() { *destroyed_ = true; }

  ObjectResolver<T> CreateResolver() {
    return ObjectResolver<T>(destroyed_, static_cast<T*>(this));
  }

 private:
  std::shared_ptr<bool> destroyed_;
};
}  // namespace cru
