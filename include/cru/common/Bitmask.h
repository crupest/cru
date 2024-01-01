#pragma once
#include <functional>

namespace cru {
template <typename Tag, typename TUnderlying = unsigned>
struct Bitmask final {
  using Underlying = TUnderlying;

  constexpr Bitmask() : value(0) {}
  constexpr explicit Bitmask(TUnderlying value) : value(value) {}

  static constexpr Bitmask FromOffset(int offset) {
    return Bitmask(static_cast<TUnderlying>(1u << offset));
  }

  Bitmask operator|(Bitmask rhs) const { return Bitmask(value | rhs.value); }
  Bitmask operator&(Bitmask rhs) const { return Bitmask(value & rhs.value); }
  Bitmask operator^(Bitmask rhs) const { return Bitmask(value ^ rhs.value); }
  Bitmask operator~() const { return Bitmask(~value); }
  Bitmask& operator|=(Bitmask rhs) {
    value |= rhs.value;
    return *this;
  }
  Bitmask& operator&=(Bitmask rhs) {
    value &= rhs.value;
    return *this;
  }
  Bitmask& operator^=(Bitmask rhs) {
    value ^= rhs.value;
    return *this;
  }

  bool operator==(Bitmask rhs) const { return this->value == rhs.value; }
  bool operator!=(Bitmask rhs) const { return this->value != rhs.value; }

  explicit operator TUnderlying() const { return value; }
  explicit operator bool() const { return value != 0; }

  TUnderlying value;
};
}  // namespace cru

namespace std {
template <typename Tag, typename TUnderlying>
struct hash<cru::Bitmask<Tag, TUnderlying>> {
  using Bitmask = cru::Bitmask<Tag, TUnderlying>;

  std::size_t operator()(Bitmask bitmask) const {
    return std::hash<TUnderlying>{}(bitmask.value);
  }
};
}  // namespace std
