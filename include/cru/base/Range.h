#pragma once
#include "Base.h"

namespace cru {
struct Range final {
  constexpr static Range FromTwoSides(Index start, Index end) {
    return Range(start, end - start);
  }

  constexpr Range() = default;
  constexpr Range(const Index position, const Index count = 0)
      : position(position), count(count) {}

  Index GetStart() const { return position; }
  Index GetEnd() const { return position + count; }

  void ChangeEnd(Index new_end) { count = new_end - position; }

  Range Normalize() const {
    auto result = *this;
    if (result.count < 0) {
      result.position += result.count;
      result.count = -result.count;
    }
    return result;
  }

  Range CoerceInto(Index min, Index max) const {
    auto coerce = [min, max](Index index) {
      return index > max ? max : (index < min ? min : index);
    };
    return Range::FromTwoSides(coerce(GetStart()), coerce(GetEnd()));
  }

  constexpr bool operator==(const Range& other) const = default;

  Index position = 0;
  Index count = 0;
};
}  // namespace cru
