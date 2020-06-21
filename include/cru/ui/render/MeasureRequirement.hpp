#pragma once
#include "Base.hpp"

#include <limits>

namespace cru::ui::render {
class MeasureLength {
 public:
  struct tag_infinate_t {};
  constexpr static tag_infinate_t tag_infinate{};

  constexpr MeasureLength() : MeasureLength(0) {}
  constexpr MeasureLength(tag_infinate_t) : length_(-1) {}
  constexpr MeasureLength(float length) : length_(length) {
    Expects(length >= 0);
  }

  MeasureLength(const MeasureLength& other) = default;
  MeasureLength& operator=(const MeasureLength& other) = default;
  MeasureLength& operator=(float length) {
    Expects(length >= 0);
    length_ = length;
    return *this;
  }

  ~MeasureLength() = default;

  constexpr static MeasureLength Infinate() {
    return MeasureLength{tag_infinate};
  }

  constexpr bool IsInfinate() const { return length_ < 0; }
  constexpr float GetLength() const {
    return length_ < 0 ? std::numeric_limits<float>::max() : length_;
  }

  constexpr bool operator==(MeasureLength other) const {
    return (length_ < 0 && other.length_ < 0) || length_ == other.length_;
  }

  constexpr bool operator!=(MeasureLength other) const {
    return !operator==(other);
  }

 private:
  // -1 for infinate
  float length_;
};

struct MeasureRequirement {
  MeasureLength max_width;
  MeasureLength max_height;

  constexpr MeasureRequirement() = default;
  constexpr MeasureRequirement(MeasureLength max_width,
                               MeasureLength max_height)
      : max_width(max_width), max_height(max_height) {}

  constexpr MeasureRequirement(const Size& max_size)
      : max_width(max_size.width), max_height(max_size.height) {}

  constexpr bool Satisfy(const Size& size) const {
    if (!max_width.IsInfinate() && max_width.GetLength() < size.width)
      return false;
    if (!max_height.IsInfinate() && max_height.GetLength() < size.height)
      return false;
    return true;
  }

  constexpr Size GetMaxSize() const {
    return Size{max_width.GetLength(), max_height.GetLength()};
  }

  constexpr static MeasureRequirement Infinate() {
    return MeasureRequirement{MeasureLength::Infinate(),
                              MeasureLength::Infinate()};
  }
};
}  // namespace cru::ui::render
