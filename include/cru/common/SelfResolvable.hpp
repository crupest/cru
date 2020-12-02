#pragma once
#include "PreConfig.hpp"

#include <memory>
#include <type_traits>

namespace cru {
template <typename T>
class SelfResolvable;

template <typename T>
class ObjectResolver {
  friend SelfResolvable<T>;

 private:
  ObjectResolver(const std::shared_ptr<T*>& resolver) : resolver_(resolver) {}

 public:
  ObjectResolver(const ObjectResolver&) = default;
  ObjectResolver& operator=(const ObjectResolver&) = default;
  ObjectResolver(ObjectResolver&&) = default;
  ObjectResolver& operator=(ObjectResolver&&) = default;
  ~ObjectResolver() = default;

  T* Resolve() const {
    // resolver_ is null only when this has been moved.
    // You shouldn't resolve a moved resolver. So assert it.
    Expects(resolver_);
    return *resolver_;
  }

 private:
  std::shared_ptr<T*> resolver_;
};

template <typename T>
class SelfResolvable {
 public:
  SelfResolvable() : resolver_(new T*(static_cast<T*>(this))) {}
  SelfResolvable(const SelfResolvable&) = delete;
  SelfResolvable& operator=(const SelfResolvable&) = delete;

  // Resolvers to old object will resolve to new object.
  SelfResolvable(SelfResolvable&& other)
      : resolver_(std::move(other.resolver_)) {
    (*resolver_) = static_cast<T*>(this);
  }

  // Old resolvers for this object will resolve to nullptr.
  // Other's resolvers will now resolve to this.
  SelfResolvable& operator=(SelfResolvable&& other) {
    if (this != &other) {
      (*resolver_) = nullptr;
      resolver_ = std::move(other.resolver_);
      (*resolver_) = static_cast<T*>(this);
    }
    return *this;
  }

  virtual ~SelfResolvable() {
    if (resolver_ != nullptr) (*resolver_) = nullptr;
  }

  ObjectResolver<T> CreateResolver() { return ObjectResolver<T>(resolver_); }

 private:
  std::shared_ptr<T*> resolver_;
};
}  // namespace cru
