#pragma once
#include "base.hpp"

namespace cru {
template <typename Tag, typename TUnderlying = unsigned>
struct Bitmask final {
  using Underlying = TUnderlying;

  constexpr Bitmask() : value(0) {}
  constexpr explicit Bitmask(TUnderlying value) : value(value) {}

  CRU_DEFAULT_COPY(Bitmask)
  CRU_DEFAULT_MOVE(Bitmask)

  ~Bitmask() = default;

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
