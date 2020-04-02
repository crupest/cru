#pragma once
#include "pre_config.hpp"

#include <gsl/gsl>

#include <stdexcept>

#define CRU_UNUSED(entity) static_cast<void>(entity);

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
  CRU_DEFAULT_COPY(Object)
  CRU_DEFAULT_MOVE(Object)
  virtual ~Object() = default;
};

struct Interface {
  Interface() = default;
  CRU_DELETE_COPY(Interface)
  CRU_DELETE_MOVE(Interface)
  virtual ~Interface() = default;
};

[[noreturn]] inline void UnreachableCode() {
  throw std::runtime_error("Unreachable code.");
}
}  // namespace cru
