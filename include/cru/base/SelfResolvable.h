#pragma once

#include <cassert>
#include <functional>
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
  template <typename U>
  using Accessor_ = std::function<U*(const std::shared_ptr<void*>&)>;
  using ThisAccessor_ = Accessor_<T>;

  explicit ObjectResolver(T* o)
      : shared_object_ptr_(new void*(o)),
        accessor_([](const std::shared_ptr<void*>& ptr) {
          return static_cast<T*>(*ptr);
        }) {}
  explicit ObjectResolver(std::shared_ptr<void*> ptr, ThisAccessor_ accessor)
      : shared_object_ptr_(std::move(ptr)), accessor_(std::move(accessor)) {}

  template <typename U>
  static ThisAccessor_ CreateAccessor(Accessor_<U> parent_accessor) {
    return [parent_accessor =
                std::move(parent_accessor)](const std::shared_ptr<void*>& ptr) {
      return static_cast<T*>(parent_accessor(ptr));
    };
  }

 public:
  template <typename U,
            typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
  ObjectResolver(const ObjectResolver<U>& other)
      : shared_object_ptr_(other.shared_object_ptr_),
        accessor_(CreateAccessor(other.accessor_)) {}

  template <typename U,
            typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
  ObjectResolver(ObjectResolver<U>&& other)
      : shared_object_ptr_(std::move(other.shared_object_ptr_)),
        accessor_(CreateAccessor(std::move(other.accessor_))) {}

  ObjectResolver(const ObjectResolver&) = default;
  ObjectResolver& operator=(const ObjectResolver&) = default;
  ObjectResolver(ObjectResolver&&) = default;
  ObjectResolver& operator=(ObjectResolver&&) = default;
  ~ObjectResolver() = default;

  template <typename U,
            typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
  ObjectResolver& operator=(const ObjectResolver<U>& other) {
    if (this != &other) {
      this->shared_object_ptr_ = other.shared_object_ptr_;
      this->accessor_ = CreateAccessor(other.accessor_);
    }
    return *this;
  }

  template <typename U,
            typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
  ObjectResolver& operator=(ObjectResolver<U>&& other) {
    if (this != &other) {
      this->shared_object_ptr_ = std::move(other.shared_object_ptr_);
      this->accessor_ = CreateAccessor(std::move(other.shared_object_ptr_));
    }
    return *this;
  }

  bool IsValid() const { return this->shared_object_ptr_ != nullptr; }

  T* Resolve() const {
    assert(IsValid());
    return this->accessor_(this->shared_object_ptr_);
  }

  /**
   * @remarks So this class can be used as a functor.
   */
  T* operator()() const { return Resolve(); }

  template <typename U,
            typename = std::enable_if_t<std::is_convertible_v<T*, U*>>>
  operator ObjectResolver<U>() const {
    return ObjectResolver<U>(*this);
  }

 private:
  void SetResolvedObject(T* o) {
    assert(IsValid());
    *this->shared_object_ptr_ = o;
  }

 private:
  std::shared_ptr<void*> shared_object_ptr_;
  std::function<T*(const std::shared_ptr<void*>&)> accessor_;
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
  SelfResolvable() : resolver_(CastToSubClass()) {}
  SelfResolvable(const SelfResolvable&) = delete;
  SelfResolvable& operator=(const SelfResolvable&) = delete;

  // Resolvers to old object will resolve to new object.
  SelfResolvable(SelfResolvable&& other)
      : resolver_(std::move(other.resolver_)) {
    this->resolver_.SetResolvedObject(CastToSubClass());
  }

  // Old resolvers for this object will resolve to nullptr.
  // Other's resolvers will now resolve to this.
  SelfResolvable& operator=(SelfResolvable&& other) {
    if (this != &other) {
      this->resolver_ = std::move(other.resolver_);
      this->resolver_.SetResolvedObject(CastToSubClass());
    }
    return *this;
  }

  virtual ~SelfResolvable() {
    if (this->resolver_.IsValid()) {
      this->resolver_.SetResolvedObject(nullptr);
    }
  }

  ObjectResolver<T> CreateResolver() { return resolver_; }

 private:
  T* CastToSubClass() { return static_cast<T*>(this); }

 private:
  ObjectResolver<T> resolver_;
};
}  // namespace cru
