#include "cru/common/StringUtil.hpp"

#include <codecvt>

namespace cru {
template <typename UInt, int number_of_bit>
inline std::enable_if_t<std::is_unsigned_v<UInt>, CodePoint> ExtractBits(
    UInt n) {
  return static_cast<CodePoint>(n & ((1u << number_of_bit) - 1));
}

CodePoint Utf16Iterator::Previous() {
  if (position_ <= 0) return k_invalid_code_point;

  const auto cu0 = static_cast<std::uint16_t>(string_[--position_]);

  if (cu0 < 0xd800u || cu0 >= 0xe000u) {  // 1-length code point
    return static_cast<CodePoint>(cu0);
  } else if (cu0 < 0xdc00u || cu0 > 0xdfffu) {  // 2-length code point
    if (position_ <= 0) {
      throw TextEncodeException(
          "Unexpected end when reading first code unit of surrogate pair "
          "during backward.");
    }
    const auto cu1 = static_cast<std::uint16_t>(string_[--position_]);

#ifdef CRU_DEBUG
    if (cu1 <= 0xdbffu) {
      throw TextEncodeException(
          "Unexpected bad-range first code unit of surrogate pair during "
          "backward.");
    }
#endif

    const auto s0 = ExtractBits<std::uint16_t, 10>(cu1) << 10;
    const auto s1 = ExtractBits<std::uint16_t, 10>(cu0);

    return s0 + s1 + 0x10000;

  } else {
    throw TextEncodeException(
        "Unexpected bad-range second code unit of surrogate pair during "
        "backward.");
  }
}

CodePoint Utf16Iterator::Next() {
  if (position_ >= static_cast<Index>(string_.length()))
    return k_invalid_code_point;

  const auto cu0 = static_cast<std::uint16_t>(string_[position_++]);

  if (cu0 < 0xd800u || cu0 >= 0xe000u) {  // 1-length code point
    return static_cast<CodePoint>(cu0);
  } else if (cu0 <= 0xdbffu) {  // 2-length code point
    if (position_ >= static_cast<Index>(string_.length())) {
      throw TextEncodeException(
          "Unexpected end when reading second code unit of surrogate pair "
          "during forward.");
    }
    const auto cu1 = static_cast<std::uint16_t>(string_[position_++]);

#ifdef CRU_DEBUG
    if (cu1 < 0xdc00u || cu1 > 0xdfffu) {
      throw TextEncodeException(
          "Unexpected bad-format second code unit of surrogate pair during "
          "forward.");
    }
#endif

    const auto s0 = ExtractBits<std::uint16_t, 10>(cu0) << 10;
    const auto s1 = ExtractBits<std::uint16_t, 10>(cu1);

    return s0 + s1 + 0x10000;

  } else {
    throw TextEncodeException(
        "Unexpected bad-format first code unit of surrogate pair during "
        "forward.");
  }
}

Index PreviousIndex(std::u16string_view string, Index current) {
  Utf16Iterator iterator{string, current};
  iterator.Previous();
  return iterator.CurrentPosition();
}

Index NextIndex(std::u16string_view string, Index current) {
  Utf16Iterator iterator{string, current};
  iterator.Next();
  return iterator.CurrentPosition();
}

std::string ToUtf8(const std::u16string& s) {
  // TODO: Implement this by myself.
  return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}
      .to_bytes(s);
}

// CodePoint Utf8Iterator::Next() {
//   if (position_ == static_cast<Index>(string_.length()))
//     return k_invalid_code_point;

//   const auto cu0 = static_cast<std::uint8_t>(string_[position_++]);

//   auto read_next_folowing_code = [this]() -> CodePoint {
//     if (this->position_ == static_cast<Index>(string_.length()))
//       throw TextEncodeException(
//           "Unexpected end when read continuing byte of multi-byte code
//           point.");

// #ifdef CRU_DEBUG
//     const auto u = static_cast<std::uint8_t>(string_[position_]);
//     if (!(u & (1u << 7)) || (u & (1u << 6))) {
//       throw TextEncodeException(
//           "Unexpected bad-format (not 0b10xxxxxx) continuing byte of "
//           "multi-byte code point.");
//     }
// #endif

//     return ExtractBits<std::uint8_t, 6>(string_[position_++]);
//   };

//   if ((1u << 7) & cu0) {
//     if ((1u << 6) & cu0) {      // 2~4-length code point
//       if ((1u << 5) & cu0) {    // 3~4-length code point
//         if ((1u << 4) & cu0) {  // 4-length code point
// #ifdef CRU_DEBUG
//           if (cu0 & (1u << 3)) {
//             throw TextEncodeException(
//                 "Unexpected bad-format begin byte (not 0b10xxxxxx) of 4-byte
//                 " "code point.");
//           }
// #endif

//           const CodePoint s0 = ExtractBits<std::uint8_t, 3>(cu0) << (6 * 3);
//           const CodePoint s1 = read_next_folowing_code() << (6 * 2);
//           const CodePoint s2 = read_next_folowing_code() << 6;
//           const CodePoint s3 = read_next_folowing_code();
//           return s0 + s1 + s2 + s3;
//         } else {  // 3-length code point
//           const CodePoint s0 = ExtractBits<std::uint8_t, 4>(cu0) << (6 * 2);
//           const CodePoint s1 = read_next_folowing_code() << 6;
//           const CodePoint s2 = read_next_folowing_code();
//           return s0 + s1 + s2;
//         }
//       } else {  // 2-length code point
//         const CodePoint s0 = ExtractBits<std::uint8_t, 5>(cu0) << 6;
//         const CodePoint s1 = read_next_folowing_code();
//         return s0 + s1;
//       }
//     } else {
//       throw TextEncodeException(
//           "Unexpected bad-format (0b10xxxxxx) begin byte of a code point.");
//     }
//   } else {
//     return static_cast<CodePoint>(cu0);
//   }
// }

}  // namespace cru
