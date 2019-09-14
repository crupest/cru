#pragma once
#include "pre_config.hpp"

#include <stdexcept>

#define CRU_DEFAULT_COPY(classname)      \
  classname(const classname&) = default; \
  classname& operator=(const classname&) = default;

#define CRU_DEFAULT_MOVE(classname) \
  classname(classname&&) = default; \
  classname& operator=(classname&&) = default;

#define CRU_DELETE_COPY(classname)      \
  classname(const classname&) = delete; \
  classname& operator=(const classname&) = delete;

#define CRU_DELETE_MOVE(classname) \
  classname(classname&&) = delete; \
  classname& operator=(classname&&) = delete;

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

[[noreturn]] inline void UnreachableCode() {
  throw std::runtime_error("Unreachable code.");
}
}  // namespace cru
