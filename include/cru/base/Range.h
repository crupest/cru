#pragma once
#include "Base.h"

#ifdef CRU_PLATFORM_OSX
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace cru {
struct Range final {
  constexpr static Range FromTwoSides(Index start, Index end) {
    return Range(start, end - start);
  }

  constexpr static Range FromTwoSides(Index start, Index end, Index offset) {
    return Range(start + offset, end - start);
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

#ifdef CRU_PLATFORM_OSX
  CFRange ToCFRange() const { return CFRangeMake(this->position, this->count); }

  static Range FromCFRange(const CFRange& range) {
    return Range(range.location, range.length);
  }
#endif

  Index position = 0;
  Index count = 0;
};
}  // namespace cru
