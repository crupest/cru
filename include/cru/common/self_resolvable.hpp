#pragma once
#include "pre_config.hpp"

#include <memory>

namespace cru {
template <typename T>
class SelfResovable;

template <typename T>
class ObjectResovler {
  friend SelfResovable<T>;

 private:
  ObjectResovler(const std::shared_ptr<T*>& resolver) : resolver_(resolver) {}

 public:
  ObjectResovler(const ObjectResovler&) = default;
  ObjectResovler& operator=(const ObjectResovler&) = default;
  ObjectResovler(ObjectResovler&&) = default;
  ObjectResovler& operator=(ObjectResovler&&) = default;
  ~ObjectResovler() = default;

  T* Resolve() const { return *resolver_; }

 private:
  std::shared_ptr<T*> resolver_;
};

template <typename T>
class SelfResovable {
 public:
  SelfResovable() : resolver_(new T*(static_cast<T*>(this))) {}
  SelfResovable(const SelfResovable&) = delete;
  SelfResovable& operator=(const SelfResovable&) = delete;
  SelfResovable(SelfResovable&&) = delete;
  SelfResovable& operator=(SelfResovable&&) = delete;
  virtual ~SelfResovable() { (*resolver_) = nullptr; }

  ObjectResovler<T> CreateResolver() { return ObjectResovler<T>(resolver_); }

 private:
  std::shared_ptr<T*> resolver_;
};
}  // namespace cru
