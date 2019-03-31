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
  virtual ~Interface() = default;
};
}  // namespace cru
