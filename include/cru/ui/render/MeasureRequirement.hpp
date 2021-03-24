#pragma once
#include "Base.hpp"

#include "cru/common/Format.hpp"

#include <fmt/core.h>
#include <algorithm>
#include <limits>
#include <string>

namespace cru::ui::render {
constexpr Size Min(const Size& left, const Size& right) {
  return Size{std::min(left.width, right.width),
              std::min(left.height, right.height)};
}

constexpr Size Max(const Size& left, const Size& right) {
  return Size{std::max(left.width, right.width),
              std::max(left.height, right.height)};
}

class MeasureLength final {
 public:
  struct tag_not_specify_t {};
  constexpr static tag_not_specify_t tag_not_specify{};

  constexpr MeasureLength() : MeasureLength(tag_not_specify) {}
  constexpr MeasureLength(tag_not_specify_t) : length_(-1) {}
  constexpr MeasureLength(float length) : length_(length) {
    Expects(length >= 0);
  }

  MeasureLength(const MeasureLength& other) = default;
  constexpr MeasureLength& operator=(const MeasureLength& other) = default;
  constexpr MeasureLength& operator=(float length) {
    Expects(length >= 0);
    length_ = length;
    return *this;
  }

  ~MeasureLength() = default;

  constexpr static MeasureLength NotSpecified() {
    return MeasureLength{tag_not_specify};
  }

  constexpr bool IsSpecified() const { return length_ >= 0.0f; }

  // What not specified means depends on situation.
  constexpr bool IsNotSpecified() const { return length_ < 0.0f; }

  constexpr float GetLengthOr(float value) const {
    return length_ < 0 ? value : length_;
  }

  // If not specify max value of float is returned.
  constexpr float GetLengthOrMax() const {
    return GetLengthOr(std::numeric_limits<float>::max());
  }

  // If not specify 0 is returned.
  constexpr float GetLengthOr0() const { return GetLengthOr(0.f); }

  // If not specify, return value is undefined.
  constexpr float GetLengthOrUndefined() const { return length_; }

  // Not operator overload because this semantics is not very clear.
  constexpr MeasureLength Plus(float length) const {
    if (IsSpecified())
      return length_ + length;
    else
      return NotSpecified();
  }

  // Not operator overload because this semantics is not very clear.
  constexpr MeasureLength Minus(float length) const {
    if (IsSpecified())
      return std::max(length_ - length, 0.f);
    else
      return NotSpecified();
  }

  constexpr bool operator==(MeasureLength other) const {
    return (length_ < 0 && other.length_ < 0) || length_ == other.length_;
  }

  constexpr bool operator!=(MeasureLength other) const {
    return !operator==(other);
  }

  constexpr static MeasureLength Min(MeasureLength left, MeasureLength right) {
    if (left.IsNotSpecified()) {
      if (right.IsNotSpecified())
        return NotSpecified();
      else
        return right;
    } else {
      if (right.IsNotSpecified())
        return left;
      else
        return std::min(left.length_, right.length_);
    }
  }

  constexpr static MeasureLength Max(MeasureLength left, MeasureLength right) {
    if (left.IsNotSpecified()) {
      if (right.IsNotSpecified())
        return NotSpecified();
      else
        return right;
    } else {
      if (left.IsNotSpecified())
        return left;
      else
        return std::max(left.length_, right.length_);
    }
  }

  std::u16string ToDebugString() const {
    return IsSpecified() ? ToUtf16String(GetLengthOrUndefined())
                         : u"UNSPECIFIED";
  }

 private:
  // -1 for not specify
  float length_;
};

struct MeasureSize {
  MeasureLength width;
  MeasureLength height;

  constexpr MeasureSize() = default;
  constexpr MeasureSize(MeasureLength width, MeasureLength height)
      : width(width), height(height) {}
  constexpr MeasureSize(const Size& size)
      : width(size.width), height(size.height) {}
  constexpr MeasureSize(const MeasureSize& other) = default;

  MeasureSize& operator=(const MeasureSize& other) = default;
  constexpr MeasureSize& operator=(const Size& other) {
    width = other.width;
    height = other.height;
    return *this;
  }

  constexpr Size GetSizeOrMax() const {
    return Size{width.GetLengthOrMax(), height.GetLengthOrMax()};
  }

  constexpr Size GetSizeOr0() const {
    return Size{width.GetLengthOr0(), height.GetLengthOr0()};
  }

  constexpr MeasureSize Plus(const Size& size) const {
    return MeasureSize{width.Plus(size.width), height.Plus(size.height)};
  }

  constexpr MeasureSize Minus(const Size& size) const {
    return MeasureSize{width.Minus(size.width), height.Minus(size.height)};
  }

  constexpr MeasureSize OverrideBy(const MeasureSize& size) const {
    return MeasureSize{
        size.width.IsSpecified() ? size.width.GetLengthOrUndefined()
                                 : this->width,
        size.height.IsSpecified() ? size.height.GetLengthOrUndefined()
                                  : this->height,
    };
  }

  std::u16string ToDebugString() const {
    return fmt::format(u"({}, {})", width.ToDebugString(),
                       height.ToDebugString());
  }

  constexpr static MeasureSize NotSpecified() {
    return MeasureSize{MeasureLength::NotSpecified(),
                       MeasureLength::NotSpecified()};
  }

  constexpr static MeasureSize Min(const MeasureSize& left,
                                   const MeasureSize& right) {
    return MeasureSize{MeasureLength::Min(left.width, right.width),
                       MeasureLength::Min(left.height, right.height)};
  }

  constexpr static MeasureSize Max(const MeasureSize& left,
                                   const MeasureSize& right) {
    return MeasureSize{MeasureLength::Max(left.width, right.width),
                       MeasureLength::Max(left.height, right.height)};
  }
};

struct MeasureRequirement {
  MeasureSize max;
  MeasureSize min;

  constexpr MeasureRequirement() = default;
  constexpr MeasureRequirement(const MeasureSize& max, const MeasureSize& min)
      : max(max), min(min) {}

  constexpr bool Satisfy(const Size& size) const {
    auto normalized = Normalize();
    return normalized.max.width.GetLengthOrMax() >= size.width &&
           normalized.max.height.GetLengthOrMax() >= size.height &&
           normalized.min.width.GetLengthOr0() <= size.width &&
           normalized.min.height.GetLengthOr0() <= size.height;
  }

  constexpr MeasureRequirement Normalize() const {
    MeasureRequirement result = *this;
    if (result.min.width.GetLengthOr0() > result.max.width.GetLengthOrMax())
      result.min.width = result.max.width;

    if (result.min.height.GetLengthOr0() > result.max.height.GetLengthOrMax())
      result.min.height = result.max.height;
    return result;
  }

  constexpr Size Coerce(const Size& size) const {
    // This order guarentees result is the same as normalized.
    return Min(Max(size, min.GetSizeOr0()), max.GetSizeOrMax());
  }

  constexpr MeasureSize Coerce(const MeasureSize& size) const {
    MeasureSize result = size;
    if (result.width.IsSpecified())
      // This order guarentees result is the same as normalized.
      result.width = std::min(std::max(min.width.GetLengthOr0(),
                                       result.width.GetLengthOrUndefined()),
                              max.width.GetLengthOrMax());

    if (result.height.IsSpecified())
      // This order guarentees result is the same as normalized.
      result.height = std::min(std::max(min.height.GetLengthOr0(),
                                        result.height.GetLengthOrUndefined()),
                               max.height.GetLengthOrMax());

    return result;
  }

  std::u16string ToDebugString() const {
    return fmt::format(u"{{min: {}, max: {}}}", min.ToDebugString(),
                       max.ToDebugString());
  }

  constexpr static MeasureRequirement Merge(const MeasureRequirement& left,
                                            const MeasureRequirement& right) {
    return MeasureRequirement{MeasureSize::Min(left.max, right.max),
                              MeasureSize::Max(left.min, right.min)};
  }
};
}  // namespace cru::ui::render
