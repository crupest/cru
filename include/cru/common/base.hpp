#pragma once
#include "pre_config.hpp"

namespace cru {
class Object {
 public:
  Object() = default;
  Object(const Object&) = default;
  Object& operator=(const Object&) = default;
  Object(Object&&) = default;
  Object& operator=(Object&&) = default;
  virtual ~Object() = default;
};

struct Interface {
  Interface() = default;
  Interface(const Interface& other) = delete;
  Interface(Interface&& other) = delete;
  Interface& operator=(const Interface& other) = delete;
  Interface& operator=(Interface&& other) = delete;
  virtual ~Interface() = default;
};
}  // namespace cru
