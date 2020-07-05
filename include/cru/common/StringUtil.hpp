#pragma once
#include "Base.hpp"

namespace cru {
using CodePoint = std::int32_t;
constexpr CodePoint k_invalid_code_point = -1;

class TextEncodeException : public std::runtime_error {
 public:
  using runtime_error::runtime_error;
};

inline bool IsSurrogatePair(char16_t c) { return c >= 0xD800 && c <= 0xDFFF; }

inline bool IsSurrogatePairLeading(char16_t c) {
  return c >= 0xD800 && c <= 0xDBFF;
}

inline bool IsSurrogatePairTrailing(char16_t c) {
  return c >= 0xDC00 && c <= 0xDFFF;
}

class Utf16Iterator : public Object {
 public:
  explicit Utf16Iterator(std::u16string_view string)
      : string_(std::move(string)) {}
  Utf16Iterator(std::u16string_view string, Index position)
      : string_(std::move(string)), position_(position) {}

  CRU_DEFAULT_COPY(Utf16Iterator)
  CRU_DEFAULT_MOVE(Utf16Iterator)

  ~Utf16Iterator() = default;

 public:
  void SetPositionToHead() { position_ = 0; }
  void SetPosition(Index position) { position_ = position; }

  // Backward current position and get previous code point. Return
  // k_invalid_code_point if reach head. Throw TextEncodeException if encounter
  // encoding problem.
  CodePoint Previous();

  // Advance current position and get next code point. Return
  // k_invalid_code_point if reach tail. Throw TextEncodeException if encounter
  // encoding problem.
  CodePoint Next();

  Index CurrentPosition() const { return this->position_; }

 private:
  std::u16string_view string_;
  Index position_ = 0;
};

Index PreviousIndex(std::u16string_view string, Index current);
Index NextIndex(std::u16string_view string, Index current);

std::string ToUtf8(const std::u16string& s);
inline std::string ToUtf8(std::u16string_view s) {
  return ToUtf8(std::u16string{s});
}

// class Utf8Iterator : public Object {
//  public:
//   explicit Utf8Iterator(const std::string_view& string) : string_(string) {}
//   Utf8Iterator(const std::string_view& string, Index position)
//       : string_(string), position_(position) {}

//   CRU_DEFAULT_COPY(Utf8Iterator)
//   CRU_DEFAULT_MOVE(Utf8Iterator)

//   ~Utf8Iterator() = default;

//  public:
//   void SetToHead() { position_ = 0; }
//   void SetPosition(Index position) { position_ = position; }

//   // Advance current position and get next code point. Return
//   k_invalid_code_point
//   // if there is no next code unit(point). Throw TextEncodeException if
//   decoding
//   // fails.
//   CodePoint Next();

//   Index CurrentPosition() const { return this->position_; }

//  private:
//   std::string_view string_;
//   Index position_ = 0;
// };
}  // namespace cru
