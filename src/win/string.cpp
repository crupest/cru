#include "cru/win/string.hpp"

#include "cru/win/exception.hpp"

#include <type_traits>

namespace cru::platform::win {
std::string ToUtf8String(const std::wstring_view& string) {
  if (string.empty()) return std::string{};

  const auto length =
      ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, string.data(), -1,
                            nullptr, 0, nullptr, nullptr);
  if (length == 0) {
    throw Win32Error(::GetLastError(),
                     "Failed to convert wide string to UTF-8.");
  }

  std::string result;
  result.resize(length);
  if (::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, string.data(), -1,
                            result.data(), static_cast<int>(result.size()),
                            nullptr, nullptr) == 0)
    throw Win32Error(::GetLastError(),
                     "Failed to convert wide string to UTF-8.");
  return result;
}

std::wstring ToUtf16String(const std::string_view& string) {
  if (string.empty()) return std::wstring{};

  const auto length = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                            string.data(), -1, nullptr, 0);
  if (length == 0) {
    throw Win32Error(::GetLastError(),
                     "Failed to convert wide string to UTF-16.");
  }

  std::wstring result;
  result.resize(length);
  if (::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, string.data(), -1,
                            result.data(),
                            static_cast<int>(result.size())) == 0)
    throw win::Win32Error(::GetLastError(),
                          "Failed to convert wide string to UTF-16.");
  return result;
}

template <typename UInt, int number_of_bit>
inline std::enable_if_t<std::is_unsigned_v<UInt>, CodePoint> ExtractBits(
    UInt n) {
  return static_cast<CodePoint>(n & ((1u << number_of_bit) - 1));
}

CodePoint Utf8Iterator::Next() {
  if (position_ == static_cast<int>(string_.length())) return k_code_point_end;

  const auto cu0 = static_cast<std::uint8_t>(string_[position_++]);

  auto read_next_folowing_code = [this]() -> CodePoint {
    if (this->position_ == static_cast<int>(string_.length()))
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

CodePoint Utf16Iterator::Next() {
  if (position_ == static_cast<int>(string_.length())) return k_code_point_end;

  const auto cu0 = static_cast<std::uint16_t>(string_[position_++]);

  if (cu0 < 0xd800u || cu0 >= 0xe000u) {  // 1-length code point
    return static_cast<CodePoint>(cu0);
  } else if (cu0 <= 0xdbffu) {  // 2-length code point
    if (position_ == static_cast<int>(string_.length())) {
      throw TextEncodeException(
          "Unexpected end when reading second code unit of surrogate pair.");
    }
    const auto cu1 = static_cast<std::uint16_t>(string_[position_++]);

#ifdef CRU_DEBUG
    if (cu1 < 0xDC00u || cu1 > 0xdfffu) {
      throw TextEncodeException(
          "Unexpected bad-format second code unit of surrogate pair.");
    }
#endif

    const auto s0 = ExtractBits<std::uint16_t, 10>(cu0) << 10;
    const auto s1 = ExtractBits<std::uint16_t, 10>(cu1);

    return s0 + s1 + 0x10000;

  } else {
    throw TextEncodeException(
        "Unexpected bad-format first code unit of surrogate pair.");
  }
}

int IndexUtf8ToUtf16(const std::string_view& utf8_string, int utf8_index,
                     const std::wstring_view& utf16_string) {
  if (utf8_index >= static_cast<int>(utf8_string.length()))
    return static_cast<int>(utf16_string.length());

  int cp_index = 0;
  Utf8Iterator iter{utf8_string};
  while (iter.CurrentPosition() <= utf8_index) {
    iter.Next();
    cp_index++;
  }

  Utf16Iterator result_iter{utf16_string};
  for (int i = 0; i < cp_index - 1; i++) {
    if (result_iter.Next() == k_code_point_end) break;
  }

  return result_iter.CurrentPosition();
}

int IndexUtf16ToUtf8(const std::wstring_view& utf16_string, int utf16_index,
                     const std::string_view& utf8_string) {
  if (utf16_index >= static_cast<int>(utf16_string.length()))
    return static_cast<int>(utf8_string.length());

  int cp_index = 0;
  Utf16Iterator iter{utf16_string};
  while (iter.CurrentPosition() <= utf16_index) {
    iter.Next();
    cp_index++;
  }

  Utf8Iterator result_iter{utf8_string};
  for (int i = 0; i < cp_index - 1; i++) {
    if (result_iter.Next() == k_code_point_end) break;
  }

  return result_iter.CurrentPosition();
}

}  // namespace cru::platform::win
