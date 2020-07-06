#include "cru/common/StringUtil.hpp"

#include <codecvt>

namespace cru {
namespace {

template <typename UInt, int number_of_bit>
inline std::enable_if_t<std::is_unsigned_v<UInt>, CodePoint> ExtractBits(
    UInt n) {
  return static_cast<CodePoint>(n & ((1u << number_of_bit) - 1));
}
}  // namespace

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
            "Unexpected end when read continuing byte of multi-byte code "
            "point.");

      const auto u = static_cast<std::uint8_t>(str[current]);
      if (!(u & (1u << 7)) || (u & (1u << 6))) {
        throw TextEncodeException(
            "Unexpected bad-format (not 0b10xxxxxx) continuing byte of "
            "multi-byte code point.");
      }

      return ExtractBits<std::uint8_t, 6>(str[current++]);
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

            const CodePoint s0 = ExtractBits<std::uint8_t, 3>(cu0) << (6 * 3);
            const CodePoint s1 = read_next_folowing_code() << (6 * 2);
            const CodePoint s2 = read_next_folowing_code() << 6;
            const CodePoint s3 = read_next_folowing_code();
            result = s0 + s1 + s2 + s3;
          } else {  // 3-length code point
            const CodePoint s0 = ExtractBits<std::uint8_t, 4>(cu0) << (6 * 2);
            const CodePoint s1 = read_next_folowing_code() << 6;
            const CodePoint s2 = read_next_folowing_code();
            result = s0 + s1 + s2;
          }
        } else {  // 2-length code point
          const CodePoint s0 = ExtractBits<std::uint8_t, 5>(cu0) << 6;
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
            "Unexpected end when reading second code unit of surrogate pair.");
      }
      const auto cu1 = str[current++];

      if (!IsUtf16SurrogatePairTrailing(cu1)) {
        throw TextEncodeException(
            "Unexpected bad-range second code unit of surrogate pair.");
      }

      const auto s0 = ExtractBits<std::uint16_t, 10>(cu0) << 10;
      const auto s1 = ExtractBits<std::uint16_t, 10>(cu1);

      result = s0 + s1 + 0x10000;

    } else {
      throw TextEncodeException(
          "Unexpected bad-range first code unit of surrogate pair.");
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
            "Unexpected end when reading first code unit of surrogate pair.");
      }
      const auto cu1 = str[--current];

      if (!IsUtf16SurrogatePairLeading(cu1)) {
        throw TextEncodeException(
            "Unexpected bad-range first code unit of surrogate pair.");
      }

      const auto s0 = ExtractBits<std::uint16_t, 10>(cu1) << 10;
      const auto s1 = ExtractBits<std::uint16_t, 10>(cu0);

      result = s0 + s1 + 0x10000;

    } else {
      throw TextEncodeException(
          "Unexpected bad-range second code unit of surrogate pair.");
    }
  }

  if (previous_position != nullptr) *previous_position = current;
  return result;
}

std::string ToUtf8(const std::u16string& s) {
  // TODO: Implement this by myself. Remember to remove deprecation warning
  // suppress macro.
  return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}
      .to_bytes(s);
}
}  // namespace cru
