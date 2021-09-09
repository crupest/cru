#pragma once
#include "Base.hpp"

namespace cru {
struct Range final {
  constexpr static Range FromTwoSides(gsl::index start, gsl::index end) {
    return Range(start, end - start);
  }

  constexpr static Range FromTwoSides(gsl::index start, gsl::index end,
                                      gsl::index offset) {
    return Range(start + offset, end - start);
  }

  constexpr Range() = default;
  constexpr Range(const gsl::index position, const gsl::index count = 0)
      : position(position), count(count) {}

  gsl::index GetStart() const { return position; }
  gsl::index GetEnd() const { return position + count; }

  void ChangeEnd(gsl::index new_end) { count = new_end - position; }

  Range Normalize() const {
    auto result = *this;
    if (result.count < 0) {
      result.position += result.count;
      result.count = -result.count;
    }
    return result;
  }

  Range CoerceInto(gsl::index min, gsl::index max) const {
    auto coerce = [min, max](gsl::index index) {
      return index > max ? max : (index < min ? min : index);
    };
    return Range::FromTwoSides(coerce(GetStart()), coerce(GetEnd()));
  }

  gsl::index position = 0;
  gsl::index count = 0;
};
}  // namespace cru
