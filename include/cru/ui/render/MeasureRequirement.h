#pragma once
#include "../Base.h"

#include <algorithm>
#include <format>
#include <limits>
#include <string>

namespace cru::ui::render {
class MeasureLength final {
 public:
  struct tag_not_specify_t {};
  constexpr static tag_not_specify_t tag_not_specify{};

  constexpr MeasureLength() : MeasureLength(tag_not_specify) {}
  constexpr MeasureLength(tag_not_specify_t) : length_(-1) {}
  constexpr MeasureLength(float length) : length_(length) {
    if (length < 0.0f) {
      throw Exception("Measure length must not be positive.");
    }
  }
  constexpr MeasureLength& operator=(float length) {
    if (length < 0.0f) {
      throw Exception("Measure length must not be positive.");
    }
    length_ = length;
    return *this;
  }

  constexpr static MeasureLength NotSpecified() {
    return MeasureLength(tag_not_specify);
  }

  constexpr bool IsSpecified() const { return length_ >= 0.0f; }

  constexpr float GetLengthOr(float value) const {
    return length_ < 0 ? value : length_;
  }

  // If not specify max value of float is returned.
  constexpr float GetLengthOrMaxFloat() const {
    return GetLengthOr(std::numeric_limits<float>::max());
  }

  // If not specify 0 is returned.
  constexpr float GetLengthOr0() const { return GetLengthOr(0.f); }

  // If not specify, return value is undefined.
  constexpr float GetLengthOrUndefined() const { return length_; }

  constexpr MeasureLength Or(MeasureLength value) const {
    return IsSpecified() ? *this : value;
  }

  constexpr MeasureLength OverrideBy(MeasureLength value) const {
    return value.IsSpecified() ? value : *this;
  }

  /**
   * If specified, given length is added and negative value is coerced to 0.
   */
  constexpr MeasureLength Plus(float length) const {
    if (IsSpecified())
      return std::max(length_ + length, 0.f);
    else
      return NotSpecified();
  }

  /**
   * If specified, given length is minused and negative value is coerced to 0.
   */
  constexpr MeasureLength Minus(float length) const {
    if (IsSpecified())
      return std::max(length_ - length, 0.f);
    else
      return NotSpecified();
  }

  /**
   * 1. Both unspecified => unspecified.
   * 2. One is specified and the other is not => the specified one.
   * 3. Both specified => smaller one.
   */
  constexpr MeasureLength Min(MeasureLength other) const {
    if (IsSpecified()) {
      return other.IsSpecified() ? std::min(length_, other.length_) : *this;
    } else {
      return other;
    }
  }

  /**
   * 1. This is unspecified => other.
   * 2. This is specified => smaller one.
   */
  constexpr float Min(float other) const {
    return IsSpecified() ? std::min(length_, other) : other;
  }

  /**
   * 1. Both unspecified => unspecified.
   * 2. One is specified and the other is not => the specified one.
   * 3. Both specified => bigger one.
   */
  constexpr MeasureLength Max(MeasureLength other) const {
    if (IsSpecified()) {
      return other.IsSpecified() ? std::max(length_, other.length_) : *this;
    } else {
      return other;
    }
  }

  /**
   * 1. This is unspecified => other.
   * 2. This is specified => bigger one.
   */
  constexpr float Max(float other) const {
    return IsSpecified() ? std::max(length_, other) : other;
  }

  std::string ToString() const {
    return IsSpecified() ? std::to_string(GetLengthOrUndefined())
                         : "unspecified";
  }

  constexpr bool operator==(const MeasureLength& other) const = default;

 private:
  // -1 for not specify
  float length_;
};
}  // namespace cru::ui::render

template <>
struct std::formatter<cru::ui::render::MeasureLength, char>
    : cru::string::ImplementFormatterByToString<
          cru::ui::render::MeasureLength> {};

namespace cru::ui::render {
struct MeasureSize {
  MeasureLength width;
  MeasureLength height;

  constexpr MeasureSize() = default;
  constexpr MeasureSize(MeasureLength width, MeasureLength height)
      : width(width), height(height) {}
  constexpr MeasureSize(const Size& size)
      : width(size.width), height(size.height) {}

  constexpr MeasureSize& operator=(const Size& other) {
    width = other.width;
    height = other.height;
    return *this;
  }

  constexpr static MeasureSize NotSpecified() {
    return {MeasureLength::NotSpecified(), MeasureLength::NotSpecified()};
  }

  constexpr Size GetSizeOrMaxFloat() const {
    return {width.GetLengthOrMaxFloat(), height.GetLengthOrMaxFloat()};
  }

  constexpr Size GetSizeOr0() const {
    return {width.GetLengthOr0(), height.GetLengthOr0()};
  }

  constexpr Size GetSizeOr(const Size& other) const {
    return {width.GetLengthOr(other.width), height.GetLengthOr(other.height)};
  }

  constexpr MeasureSize Or(const MeasureSize& other) const {
    return {width.Or(other.width), height.Or(other.height)};
  }

  constexpr MeasureSize OverrideBy(const MeasureSize& other) const {
    return {width.OverrideBy(other.width), height.OverrideBy(other.height)};
  }

  constexpr MeasureSize Plus(const Size& size) const {
    return {width.Plus(size.width), height.Plus(size.height)};
  }

  constexpr MeasureSize Minus(const Size& size) const {
    return {width.Minus(size.width), height.Minus(size.height)};
  }

  constexpr MeasureSize Min(const MeasureSize& other) const {
    return {width.Min(other.width), height.Min(other.height)};
  }

  constexpr Size Min(const Size& other) const {
    return {width.Min(other.width), height.Min(other.height)};
  }

  constexpr MeasureSize Max(const MeasureSize& other) const {
    return {width.Max(other.width), height.Max(other.height)};
  }

  constexpr Size Max(const Size& other) const {
    return {width.Max(other.width), height.Max(other.height)};
  }

  std::string ToString() const { return std::format("{}x{}", width, height); }

  constexpr bool operator==(const MeasureSize& other) const = default;
};
}  // namespace cru::ui::render

template <>
struct std::formatter<cru::ui::render::MeasureSize, char>
    : cru::string::ImplementFormatterByToString<cru::ui::render::MeasureSize> {
};

namespace cru::ui::render {
struct MeasureRequirement {
  MeasureSize max;
  MeasureSize min;
  MeasureSize suggest;

  constexpr MeasureRequirement() = default;
  constexpr MeasureRequirement(const MeasureSize& max, const MeasureSize& min,
                               const MeasureSize& suggest,
                               bool allow_coerce_suggest = true)
      : max(max), min(min), suggest(Coerce(suggest)) {
    if (max.width.GetLengthOrMaxFloat() < min.width.GetLengthOr0()) {
      throw Exception(
          "Measure requirement's max width is smaller than min width.");
    }

    if (max.height.GetLengthOrMaxFloat() < min.height.GetLengthOr0()) {
      throw Exception(
          "Measure requirement's max height is smaller than min height.");
    }

    if (!allow_coerce_suggest && this->suggest != suggest) {
      throw Exception(
          "Measure requirement's suggest size is in invalid range.");
    }
  }

  constexpr bool Satisfy(const Size& size) const {
    return max.width.GetLengthOrMaxFloat() >= size.width &&
           max.height.GetLengthOrMaxFloat() >= size.height &&
           min.width.GetLengthOr0() <= size.width &&
           min.height.GetLengthOr0() <= size.height;
  }

  constexpr Size Coerce(const Size& size) const {
    return max.Min(min.Max(size));
  }

  constexpr Size ExpandToSuggestAndCoerce(const Size& size) const {
    return max.Min(min.Max(suggest.Max(size)));
  }

  constexpr MeasureSize Coerce(const MeasureSize& size) const {
    MeasureSize result = size;
    if (result.width.IsSpecified())
      result.width =
          max.width.Min(min.width.Max(result.width.GetLengthOrUndefined()));

    if (result.height.IsSpecified())
      result.height =
          max.height.Min(min.height.Max(result.height.GetLengthOrUndefined()));

    return result;
  }

  constexpr MeasureRequirement Minus(const Size& size) const {
    return {max.Minus(size), min.Minus(size), suggest.Minus(size)};
  }

  /**
   * Suggest size will use the other's one if this doesn't specify one but the
   * other does.
   */
  constexpr MeasureRequirement Merge(const MeasureRequirement& other) const {
    return {max.Min(other.max), min.Max(other.min),
            suggest.OverrideBy(other.suggest)};
  }

  std::string ToString() const {
    return std::format("(min: {}, max: {}, suggest: {})", min, max, suggest);
  }

  constexpr bool operator==(const MeasureRequirement& other) const = default;
};
}  // namespace cru::ui::render

template <>
struct std::formatter<cru::ui::render::MeasureRequirement, char>
    : cru::string::ImplementFormatterByToString<
          cru::ui::render::MeasureRequirement> {};
