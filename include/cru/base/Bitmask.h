#pragma once

#include <functional>

namespace cru {
template <typename Tag, typename TUnderlying = unsigned>
struct Bitmask final {
  using Underlying = TUnderlying;

  constexpr Bitmask() : value(0) {}
  constexpr explicit Bitmask(Underlying value) : value(value) {}

  // Start from 1.
  static constexpr Bitmask FromOffset(int offset) {
    if (offset == 0) return {};
    return Bitmask(static_cast<Underlying>(Underlying(1) << (offset - 1)));
  }

  constexpr bool Has(Bitmask rhs) const { return (value & rhs.value) != 0; }

  constexpr Bitmask operator|(Bitmask rhs) const {
    return Bitmask(value | rhs.value);
  }
  constexpr Bitmask operator&(Bitmask rhs) const {
    return Bitmask(value & rhs.value);
  }
  constexpr Bitmask operator^(Bitmask rhs) const {
    return Bitmask(value ^ rhs.value);
  }
  constexpr Bitmask operator~() const { return Bitmask(~value); }
  constexpr Bitmask& operator|=(Bitmask rhs) {
    value |= rhs.value;
    return *this;
  }
  constexpr Bitmask& operator&=(Bitmask rhs) {
    value &= rhs.value;
    return *this;
  }
  constexpr Bitmask& operator^=(Bitmask rhs) {
    value ^= rhs.value;
    return *this;
  }

  constexpr bool operator==(Bitmask rhs) const {
    return this->value == rhs.value;
  }

  constexpr explicit operator Underlying() const { return value; }
  constexpr explicit operator bool() const { return value != 0; }

  Underlying value;
};
}  // namespace cru

namespace std {
template <typename Tag, typename TUnderlying>
struct hash<cru::Bitmask<Tag, TUnderlying>> {
  constexpr std::size_t operator()(
      cru::Bitmask<Tag, TUnderlying> bitmask) const {
    return std::hash<TUnderlying>{}(bitmask.value);
  }
};
}  // namespace std

#define CRU_DEFINE_BITMASK(name)                   \
  namespace details {                              \
  struct name##Tag {};                             \
  }                                                \
  using name = ::cru::Bitmask<details::name##Tag>; \
  struct name##s
