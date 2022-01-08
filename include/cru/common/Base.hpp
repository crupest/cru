#pragma once
#include "PreConfig.hpp"

#ifdef CRU_PLATFORM_WINDOWS
#ifdef CRU_BASE_EXPORT_API
#define CRU_BASE_API __declspec(dllexport)
#else
#define CRU_BASE_API __declspec(dllimport)
#endif
#else
#define CRU_BASE_API
#endif

#include <gsl/gsl>
#include <stdexcept>
#include <string>

#define CRU_UNUSED(entity) static_cast<void>(entity);

#define CRU__CONCAT(a, b) a##b
#define CRU_MAKE_UNICODE_LITERAL(str) CRU__CONCAT(u, #str)

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

#define CRU_DEFAULT_DESTRUCTOR(classname) ~classname() = default;

#define CRU_DEFAULT_CONSTRUCTOR_DESTRUCTOR(classname) \
  classname() = default;                              \
  ~classname() = default;

#define CRU_DEFINE_COMPARE_OPERATORS(classname)                           \
  inline bool operator==(const classname& left, const classname& right) { \
    return left.Compare(right) == 0;                                      \
  }                                                                       \
                                                                          \
  inline bool operator!=(const classname& left, const classname& right) { \
    return left.Compare(right) != 0;                                      \
  }                                                                       \
                                                                          \
  inline bool operator<(const classname& left, const classname& right) {  \
    return left.Compare(right) < 0;                                       \
  }                                                                       \
                                                                          \
  inline bool operator<=(const classname& left, const classname& right) { \
    return left.Compare(right) <= 0;                                      \
  }                                                                       \
                                                                          \
  inline bool operator>(const classname& left, const classname& right) {  \
    return left.Compare(right) > 0;                                       \
  }                                                                       \
                                                                          \
  inline bool operator>=(const classname& left, const classname& right) { \
    return left.Compare(right) >= 0;                                      \
  }

namespace cru {
class CRU_BASE_API Object {
 public:
  Object() = default;
  CRU_DEFAULT_COPY(Object)
  CRU_DEFAULT_MOVE(Object)
  virtual ~Object() = default;
};

struct CRU_BASE_API Interface {
  Interface() = default;
  CRU_DELETE_COPY(Interface)
  CRU_DELETE_MOVE(Interface)
  virtual ~Interface() = default;
};

[[noreturn]] inline void UnreachableCode() { std::terminate(); }

using Index = gsl::index;

// https://www.boost.org/doc/libs/1_54_0/doc/html/hash/reference.html#boost.hash_combine
template <class T>
inline void hash_combine(std::size_t& s, const T& v) {
  std::hash<T> h;
  s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

#define CRU_DEFINE_CLASS_LOG_TAG(tag) \
 private:                             \
  constexpr static StringView log_tag = tag;
}  // namespace cru
