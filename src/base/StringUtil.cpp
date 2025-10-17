#include "cru/base/StringUtil.h"
#include "cru/base/Base.h"
#include "cru/base/Exception.h"

#include <algorithm>
#include <cctype>
#include <compare>

namespace cru {
namespace string {

std::weak_ordering CaseInsensitiveCompare(std::string_view left,
                                          std::string_view right) {
  return std::lexicographical_compare_three_way(
      left.cbegin(), left.cend(), right.cbegin(), right.cend(),
      [](char left, char right) {
        auto l = tolower(left), r = tolower(right);
        return l < r ? std::weak_ordering::less
                     : (l == r ? std::weak_ordering::equivalent
                               : std::weak_ordering::greater);
      });
}

std::string TrimBegin(std::string_view str) {
  auto iter = std::find_if(str.cbegin(), str.cend(),
                           [](char c) { return !std::isspace(c); });
  return std::string(iter, str.cend());
}

std::string TrimEnd(std::string_view str) {
  auto iter = std::find_if(str.crbegin(), str.crend(),
                           [](char c) { return !std::isspace(c); });
  return std::string(str.cbegin(), str.cend() - (iter - str.crbegin()));
}

std::string Trim(std::string_view str) {
  auto iter1 = std::find_if(str.cbegin(), str.cend(),
                            [](char c) { return !std::isspace(c); });
  auto iter2 = std::find_if(str.crbegin(), str.crend(),
                            [](char c) { return !std::isspace(c); });
  return std::string(iter1, str.cend() - (iter2 - str.crbegin()));
}
}  // namespace string

using details::ExtractBits;

CodePoint Utf8NextCodePoint(const char* ptr, Index size, Index current,
                            Index* next_position) {
  CodePoint result;

  if (current >= size) {
    result = k_invalid_code_point;
  } else {
    const auto cu0 = static_cast<std::uint8_t>(ptr[current++]);

    auto read_next_folowing_code = [ptr, size, &current]() -> CodePoint {
      if (current == size)
        throw TextEncodeException(
            "Unexpected end when read continuing byte of multi-byte code "
            "point.");

      const auto u = static_cast<std::uint8_t>(ptr[current]);
      if (!(u & (1u << 7)) || (u & (1u << 6))) {
        throw TextEncodeException(
            "Unexpected bad-format (not 0b10xxxxxx) continuing byte of "
            "multi-byte code point.");
      }

      return ExtractBits<std::uint8_t, 6, CodePoint>(ptr[current++]);
    };

    if ((1u << 7) & cu0) {
      if ((1u << 6) & cu0) {      // 2~4-length code point
        if ((1u << 5) & cu0) {    // 3~4-length code point
          if ((1u << 4) & cu0) {  // 4-length code point
            if (cu0 & (1u << 3)) {
              throw TextEncodeException(
                  "Unexpected bad-format begin byte (not 0b11110xxx) of 4-byte"
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
            "Unexpected bad-format (0b10xxxxxx) begin byte of a code point.");
      }
    } else {
      result = static_cast<CodePoint>(cu0);
    }
  }

  if (next_position != nullptr) *next_position = current;
  return result;
}

CodePoint Utf16NextCodePoint(const char16_t* ptr, Index size, Index current,
                             Index* next_position) {
  CodePoint result;

  if (current >= size) {
    result = k_invalid_code_point;
  } else {
    const auto cu0 = ptr[current++];

    if (!IsUtf16SurrogatePairCodeUnit(cu0)) {  // 1-length code point
      result = static_cast<CodePoint>(cu0);
    } else if (IsUtf16SurrogatePairLeading(cu0)) {  // 2-length code point
      if (current >= size) {
        throw TextEncodeException(
            "Unexpected end when reading second code unit of surrogate pair.");
      }
      const auto cu1 = ptr[current++];

      if (!IsUtf16SurrogatePairTrailing(cu1)) {
        throw TextEncodeException(
            "Unexpected bad-range second code unit of surrogate pair.");
      }

      const auto s0 = ExtractBits<std::uint16_t, 10, CodePoint>(cu0) << 10;
      const auto s1 = ExtractBits<std::uint16_t, 10, CodePoint>(cu1);

      result = s0 + s1 + 0x10000;

    } else {
      throw TextEncodeException(
          "Unexpected bad-range first code unit of surrogate pair.");
    }
  }

  if (next_position != nullptr) *next_position = current;
  return result;
}

CodePoint Utf16PreviousCodePoint(const char16_t* ptr, Index size, Index current,
                                 Index* previous_position) {
  CRU_UNUSED(size)

  CodePoint result;
  if (current <= 0) {
    result = k_invalid_code_point;
  } else {
    const auto cu0 = ptr[--current];

    if (!IsUtf16SurrogatePairCodeUnit(cu0)) {  // 1-length code point
      result = static_cast<CodePoint>(cu0);
    } else if (IsUtf16SurrogatePairTrailing(cu0)) {  // 2-length code point
      if (current <= 0) {
        throw TextEncodeException(
            "Unexpected end when reading first code unit of surrogate pair.");
      }
      const auto cu1 = ptr[--current];

      if (!IsUtf16SurrogatePairLeading(cu1)) {
        throw TextEncodeException(
            "Unexpected bad-range first code unit of surrogate pair.");
      }

      const auto s0 = ExtractBits<std::uint16_t, 10, CodePoint>(cu1) << 10;
      const auto s1 = ExtractBits<std::uint16_t, 10, CodePoint>(cu0);

      result = s0 + s1 + 0x10000;

    } else {
      throw TextEncodeException(
          "Unexpected bad-range second code unit of surrogate pair.");
    }
  }

  if (previous_position != nullptr) *previous_position = current;
  return result;
}

bool Utf16IsValidInsertPosition(const char16_t* ptr, Index size,
                                Index position) {
  if (position < 0) return false;
  if (position > size) return false;
  if (position == 0) return true;
  if (position == size) return true;
  return !IsUtf16SurrogatePairTrailing(ptr[position]);
}

Index Utf16BackwardUntil(const char16_t* ptr, Index size, Index position,
                         const std::function<bool(CodePoint)>& predicate) {
  if (position <= 0) return position;
  while (true) {
    Index p = position;
    auto c = Utf16PreviousCodePoint(ptr, size, p, &position);
    if (predicate(c)) return p;
    if (c == k_invalid_code_point) return p;
  }
  UnreachableCode();
}

Index Utf16ForwardUntil(const char16_t* ptr, Index size, Index position,
                        const std::function<bool(CodePoint)>& predicate) {
  if (position >= size) return position;
  while (true) {
    Index p = position;
    auto c = Utf16NextCodePoint(ptr, size, p, &position);
    if (predicate(c)) return p;
    if (c == k_invalid_code_point) return p;
  }
  UnreachableCode();
}

inline bool IsSpace(CodePoint c) { return c == 0x20 || c == 0xA; }

Index Utf16PreviousWord(const char16_t* ptr, Index size, Index position,
                        bool* is_space) {
  if (position <= 0) return position;
  auto c = Utf16PreviousCodePoint(ptr, size, position, nullptr);
  if (IsSpace(c)) {  // TODO: Currently only test against 0x20(space).
    if (is_space) *is_space = true;
    return Utf16BackwardUntil(ptr, size, position,
                              [](CodePoint c) { return !IsSpace(c); });
  } else {
    if (is_space) *is_space = false;
    return Utf16BackwardUntil(ptr, size, position, IsSpace);
  }
}

Index Utf16NextWord(const char16_t* ptr, Index size, Index position,
                    bool* is_space) {
  if (position >= size) return position;
  auto c = Utf16NextCodePoint(ptr, size, position, nullptr);
  if (IsSpace(c)) {  // TODO: Currently only test against 0x20(space).
    if (is_space) *is_space = true;
    return Utf16ForwardUntil(ptr, size, position,
                             [](CodePoint c) { return !IsSpace(c); });
  } else {
    if (is_space) *is_space = false;
    return Utf16ForwardUntil(ptr, size, position, IsSpace);
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

bool IsWhitespace(char16_t c) {
  return c == u' ' || c == u'\t' || c == u'\n' || c == u'\r';
}

bool IsDigit(char16_t c) { return c >= u'0' && c <= u'9'; }

Utf8CodePointIterator CreateUtf8Iterator(const std::byte* buffer, Index size) {
  return Utf8CodePointIterator(reinterpret_cast<const char*>(buffer), size);
}

Utf8CodePointIterator CreateUtf8Iterator(const std::vector<std::byte>& buffer) {
  return CreateUtf8Iterator(buffer.data(), buffer.size());
}

}  // namespace cru
