#include "cru/common/StringUtil.hpp"
#include <functional>
#include "cru/common/Base.hpp"
#include "cru/common/Exception.hpp"

namespace cru {
using details::ExtractBits;

CodePoint Utf8NextCodePoint(std::string_view str, Index current,
                            Index* next_position) {
  CodePoint result;

  if (current >= static_cast<Index>(str.length())) {
    result = k_invalid_code_point;
  } else {
    const auto cu0 = static_cast<std::uint8_t>(str[current++]);

    auto read_next_folowing_code = [&str, &current]() -> CodePoint {
      if (current == static_cast<Index>(str.length()))
        throw TextEncodeException(
            u"Unexpected end when read continuing byte of multi-byte code "
            "point.");

      const auto u = static_cast<std::uint8_t>(str[current]);
      if (!(u & (1u << 7)) || (u & (1u << 6))) {
        throw TextEncodeException(
            u"Unexpected bad-format (not 0b10xxxxxx) continuing byte of "
            "multi-byte code point.");
      }

      return ExtractBits<std::uint8_t, 6, CodePoint>(str[current++]);
    };

    if ((1u << 7) & cu0) {
      if ((1u << 6) & cu0) {      // 2~4-length code point
        if ((1u << 5) & cu0) {    // 3~4-length code point
          if ((1u << 4) & cu0) {  // 4-length code point
            if (cu0 & (1u << 3)) {
              throw TextEncodeException(
                  u"Unexpected bad-format begin byte (not 0b11110xxx) of 4-byte"
                  "code point.");
            }

            const CodePoint s0 = ExtractBits<std::uint8_t, 3, CodePoint>(cu0)
                                 << (6 * 3);
            const CodePoint s1 = read_next_folowing_code() << (6 * 2);
            const CodePoint s2 = read_next_folowing_code() << 6;
            const CodePoint s3 = read_next_folowing_code();
            result = s0 + s1 + s2 + s3;
          } else {  // 3-length code point
            const CodePoint s0 = ExtractBits<std::uint8_t, 4, CodePoint>(cu0)
                                 << (6 * 2);
            const CodePoint s1 = read_next_folowing_code() << 6;
            const CodePoint s2 = read_next_folowing_code();
            result = s0 + s1 + s2;
          }
        } else {  // 2-length code point
          const CodePoint s0 = ExtractBits<std::uint8_t, 5, CodePoint>(cu0)
                               << 6;
          const CodePoint s1 = read_next_folowing_code();
          result = s0 + s1;
        }
      } else {
        throw TextEncodeException(
            u"Unexpected bad-format (0b10xxxxxx) begin byte of a code point.");
      }
    } else {
      result = static_cast<CodePoint>(cu0);
    }
  }

  if (next_position != nullptr) *next_position = current;
  return result;
}

CodePoint Utf16NextCodePoint(std::u16string_view str, Index current,
                             Index* next_position) {
  CodePoint result;

  if (current >= static_cast<Index>(str.length())) {
    result = k_invalid_code_point;
  } else {
    const auto cu0 = str[current++];

    if (!IsUtf16SurrogatePairCodeUnit(cu0)) {  // 1-length code point
      result = static_cast<CodePoint>(cu0);
    } else if (IsUtf16SurrogatePairLeading(cu0)) {  // 2-length code point
      if (current >= static_cast<Index>(str.length())) {
        throw TextEncodeException(
            u"Unexpected end when reading second code unit of surrogate pair.");
      }
      const auto cu1 = str[current++];

      if (!IsUtf16SurrogatePairTrailing(cu1)) {
        throw TextEncodeException(
            u"Unexpected bad-range second code unit of surrogate pair.");
      }

      const auto s0 = ExtractBits<std::uint16_t, 10, CodePoint>(cu0) << 10;
      const auto s1 = ExtractBits<std::uint16_t, 10, CodePoint>(cu1);

      result = s0 + s1 + 0x10000;

    } else {
      throw TextEncodeException(
          u"Unexpected bad-range first code unit of surrogate pair.");
    }
  }

  if (next_position != nullptr) *next_position = current;
  return result;
}

CodePoint Utf16PreviousCodePoint(std::u16string_view str, Index current,
                                 Index* previous_position) {
  CodePoint result;
  if (current <= 0) {
    result = k_invalid_code_point;
  } else {
    const auto cu0 = str[--current];

    if (!IsUtf16SurrogatePairCodeUnit(cu0)) {  // 1-length code point
      result = static_cast<CodePoint>(cu0);
    } else if (IsUtf16SurrogatePairTrailing(cu0)) {  // 2-length code point
      if (current <= 0) {
        throw TextEncodeException(
            u"Unexpected end when reading first code unit of surrogate pair.");
      }
      const auto cu1 = str[--current];

      if (!IsUtf16SurrogatePairLeading(cu1)) {
        throw TextEncodeException(
            u"Unexpected bad-range first code unit of surrogate pair.");
      }

      const auto s0 = ExtractBits<std::uint16_t, 10, CodePoint>(cu1) << 10;
      const auto s1 = ExtractBits<std::uint16_t, 10, CodePoint>(cu0);

      result = s0 + s1 + 0x10000;

    } else {
      throw TextEncodeException(
          u"Unexpected bad-range second code unit of surrogate pair.");
    }
  }

  if (previous_position != nullptr) *previous_position = current;
  return result;
}

void Utf8EncodeCodePointAppend(CodePoint code_point, std::string& str) {
  if (!Utf8EncodeCodePointAppendWithFunc(code_point,
                                         [&str](char c) { str.push_back(c); }))
    throw TextEncodeException(u"Code point out of range.");
}

void Utf16EncodeCodePointAppend(CodePoint code_point, std::u16string& str) {
  if (!Utf16EncodeCodePointAppendWithFunc(
          code_point, [&str](char16_t c) { str.push_back(c); }))
    throw TextEncodeException(u"Code point out of range.");
}

std::string ToUtf8(std::u16string_view s) {
  std::string result;
  for (CodePoint cp : Utf16CodePointIterator{s}) {
    Utf8EncodeCodePointAppend(cp, result);
  }
  return result;
}

std::u16string ToUtf16(std::string_view s) {
  std::u16string result;
  for (CodePoint cp : Utf8CodePointIterator{s}) {
    Utf16EncodeCodePointAppend(cp, result);
  }
  return result;
}

bool Utf16IsValidInsertPosition(std::u16string_view s, gsl::index position) {
  if (position < 0) return false;
  if (position > static_cast<gsl::index>(s.size())) return false;
  if (position == 0) return true;
  if (position == static_cast<gsl::index>(s.size())) return true;
  return !IsUtf16SurrogatePairTrailing(s[position]);
}

gsl::index Utf16BackwardUntil(std::u16string_view str, gsl::index position,
                              const std::function<bool(CodePoint)>& predicate) {
  if (position <= 0) return position;
  while (true) {
    gsl::index p = position;
    auto c = Utf16PreviousCodePoint(str, p, &position);
    if (predicate(c)) return p;
    if (c == k_invalid_code_point) return p;
  }
  UnreachableCode();
}

gsl::index Utf16ForwardUntil(std::u16string_view str, gsl::index position,
                             const std::function<bool(CodePoint)>& predicate) {
  if (position >= static_cast<gsl::index>(str.size())) return position;
  while (true) {
    gsl::index p = position;
    auto c = Utf16NextCodePoint(str, p, &position);
    if (predicate(c)) return p;
    if (c == k_invalid_code_point) return p;
  }
  UnreachableCode();
}

inline bool IsSpace(CodePoint c) { return c == 0x20 || c == 0xA; }

gsl::index Utf16PreviousWord(std::u16string_view str, gsl::index position,
                             bool* is_space) {
  if (position <= 0) return position;
  auto c = Utf16PreviousCodePoint(str, position, nullptr);
  if (IsSpace(c)) {  // TODO: Currently only test against 0x20(space).
    if (is_space) *is_space = true;
    return Utf16BackwardUntil(str, position,
                              [](CodePoint c) { return !IsSpace(c); });
  } else {
    if (is_space) *is_space = false;
    return Utf16BackwardUntil(str, position, IsSpace);
  }
}

gsl::index Utf16NextWord(std::u16string_view str, gsl::index position,
                         bool* is_space) {
  if (position >= static_cast<gsl::index>(str.size())) return position;
  auto c = Utf16NextCodePoint(str, position, nullptr);
  if (IsSpace(c)) {  // TODO: Currently only test against 0x20(space).
    if (is_space) *is_space = true;
    return Utf16ForwardUntil(str, position,
                             [](CodePoint c) { return !IsSpace(c); });
  } else {
    if (is_space) *is_space = false;
    return Utf16ForwardUntil(str, position, IsSpace);
  }
}

char16_t ToLower(char16_t c) {
  if (c >= u'A' && c <= u'Z') {
    return c - u'A' + u'a';
  }
  return c;
}

char16_t ToUpper(char16_t c) {
  if (c >= u'a' && c <= u'z') {
    return c - u'a' + u'A';
  }
  return c;
}

std::u16string ToLower(std::u16string_view s) {
  std::u16string result;
  for (auto c : s) result.push_back(ToLower(c));
  return result;
}

std::u16string ToUpper(std::u16string_view s) {
  std::u16string result;
  for (auto c : s) result.push_back(ToUpper(c));
  return result;
}
}  // namespace cru
