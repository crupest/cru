#pragma once
#include "pre_config.hpp"

#include <stdexcept>
#include <type_traits>

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

template <typename Enum>
struct enable_bitmask_operators : std::false_type {};

#define CRU_ENABLE_BITMASK_OPERATORS(enumname) \
  template <>                                  \
  struct enable_bitmask_operators<enumname> : std::true_type {};

template <typename Enum>
auto operator|(Enum lhs, Enum rhs)
    -> std::enable_if_t<enable_bitmask_operators<Enum>::value, Enum> {
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum>(static_cast<underlying>(lhs) |
                           static_cast<underlying>(rhs));
}

template <typename Enum>
auto operator&(Enum lhs, Enum rhs)
    -> std::enable_if_t<enable_bitmask_operators<Enum>::value, Enum> {
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum>(static_cast<underlying>(lhs) &
                           static_cast<underlying>(rhs));
}

template <typename Enum>
auto operator^(Enum lhs, Enum rhs)
    -> std::enable_if_t<enable_bitmask_operators<Enum>::value, Enum> {
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum>(static_cast<underlying>(lhs) ^
                           static_cast<underlying>(rhs));
}

template <typename Enum>
auto operator~(Enum rhs)
    -> std::enable_if_t<enable_bitmask_operators<Enum>::value, Enum> {
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum>(~static_cast<underlying>(rhs));
}

template <typename Enum>
auto operator|=(Enum& lhs, Enum rhs)
    -> std::enable_if_t<enable_bitmask_operators<Enum>::value, Enum&> {
  using underlying = typename std::underlying_type<Enum>::type;
  lhs = static_cast<Enum>(static_cast<underlying>(lhs) |
                          static_cast<underlying>(rhs));
  return lhs;
}

template <typename Enum>
auto operator&=(Enum& lhs, Enum rhs)
    -> std::enable_if_t<enable_bitmask_operators<Enum>::value, Enum&> {
  using underlying = typename std::underlying_type<Enum>::type;
  lhs = static_cast<Enum>(static_cast<underlying>(lhs) &
                          static_cast<underlying>(rhs));
  return lhs;
}

template <typename Enum>
auto operator^=(Enum& lhs, Enum rhs)
    -> std::enable_if_t<enable_bitmask_operators<Enum>::value, Enum&> {
  using underlying = typename std::underlying_type<Enum>::type;
  lhs = static_cast<Enum>(static_cast<underlying>(lhs) ^
                          static_cast<underlying>(rhs));
  return lhs;
}
