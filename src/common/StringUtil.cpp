#include "cru/common/StringUtil.hpp"

namespace cru {
template <typename UInt, int number_of_bit>
inline std::enable_if_t<std::is_unsigned_v<UInt>, CodePoint> ExtractBits(
    UInt n) {
  return static_cast<CodePoint>(n & ((1u << number_of_bit) - 1));
}

CodePoint Utf8Iterator::Next() {
  if (position_ == static_cast<Index>(string_.length()))
    return k_code_point_end;

  const auto cu0 = static_cast<std::uint8_t>(string_[position_++]);

  auto read_next_folowing_code = [this]() -> CodePoint {
    if (this->position_ == static_cast<Index>(string_.length()))
      throw TextEncodeException(
          "Unexpected end when read continuing byte of multi-byte code point.");

#ifdef CRU_DEBUG
    const auto u = static_cast<std::uint8_t>(string_[position_]);
    if (!(u & (1u << 7)) || (u & (1u << 6))) {
      throw TextEncodeException(
          "Unexpected bad-format (not 0b10xxxxxx) continuing byte of "
          "multi-byte code point.");
    }
#endif

    return ExtractBits<std::uint8_t, 6>(string_[position_++]);
  };

  if ((1u << 7) & cu0) {
    if ((1u << 6) & cu0) {      // 2~4-length code point
      if ((1u << 5) & cu0) {    // 3~4-length code point
        if ((1u << 4) & cu0) {  // 4-length code point
#ifdef CRU_DEBUG
          if (cu0 & (1u << 3)) {
            throw TextEncodeException(
                "Unexpected bad-format begin byte (not 0b10xxxxxx) of 4-byte "
                "code point.");
          }
#endif

          const CodePoint s0 = ExtractBits<std::uint8_t, 3>(cu0) << (6 * 3);
          const CodePoint s1 = read_next_folowing_code() << (6 * 2);
          const CodePoint s2 = read_next_folowing_code() << 6;
          const CodePoint s3 = read_next_folowing_code();
          return s0 + s1 + s2 + s3;
        } else {  // 3-length code point
          const CodePoint s0 = ExtractBits<std::uint8_t, 4>(cu0) << (6 * 2);
          const CodePoint s1 = read_next_folowing_code() << 6;
          const CodePoint s2 = read_next_folowing_code();
          return s0 + s1 + s2;
        }
      } else {  // 2-length code point
        const CodePoint s0 = ExtractBits<std::uint8_t, 5>(cu0) << 6;
        const CodePoint s1 = read_next_folowing_code();
        return s0 + s1;
      }
    } else {
      throw TextEncodeException(
          "Unexpected bad-format (0b10xxxxxx) begin byte of a code point.");
    }
  } else {
    return static_cast<CodePoint>(cu0);
  }
}
}  // namespace cru
