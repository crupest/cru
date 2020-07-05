#include "cru/win/String.hpp"

#include "cru/win/Exception.hpp"

#include <type_traits>

namespace cru::platform::win {
std::string ToUtf8String(const std::wstring_view& string) {
  if (string.empty()) return std::string{};

  const auto length = ::WideCharToMultiByte(
      CP_UTF8, WC_ERR_INVALID_CHARS, string.data(),
      static_cast<int>(string.size()), nullptr, 0, nullptr, nullptr);
  if (length == 0) {
    throw Win32Error(::GetLastError(),
                     "Failed to convert wide string to UTF-8.");
  }

  std::string result;
  result.resize(length);
  if (::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, string.data(),
                            static_cast<int>(string.size()), result.data(),
                            static_cast<int>(result.size()), nullptr,
                            nullptr) == 0)
    throw Win32Error(::GetLastError(),
                     "Failed to convert wide string to UTF-8.");
  return result;
}

std::wstring ToUtf16String(const std::string_view& string) {
  if (string.empty()) return std::wstring{};

  const auto length =
      ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, string.data(),
                            static_cast<int>(string.size()), nullptr, 0);
  if (length == 0) {
    throw Win32Error(::GetLastError(),
                     "Failed to convert wide string to UTF-16.");
  }

  std::wstring result;
  result.resize(length);
  if (::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, string.data(),
                            static_cast<int>(string.size()), result.data(),
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

CodePoint Utf16Iterator::Next() {
  if (position_ == static_cast<Index>(string_.length()))
    return k_code_point_end;

  const auto cu0 = static_cast<std::uint16_t>(string_[position_++]);

  if (cu0 < 0xd800u || cu0 >= 0xe000u) {  // 1-length code point
    return static_cast<CodePoint>(cu0);
  } else if (cu0 <= 0xdbffu) {  // 2-length code point
    if (position_ == static_cast<Index>(string_.length())) {
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

Index IndexUtf8ToUtf16(const std::string_view& utf8_string, Index utf8_index,
                       const std::wstring_view& utf16_string) {
  if (utf8_index >= static_cast<Index>(utf8_string.length()))
    return utf16_string.length();

  Index cp_index = 0;
  Utf8Iterator iter{utf8_string};
  while (iter.CurrentPosition() <= utf8_index) {
    iter.Next();
    cp_index++;
  }

  Utf16Iterator result_iter{utf16_string};
  for (Index i = 0; i < cp_index - 1; i++) {
    if (result_iter.Next() == k_code_point_end) break;
  }

  return result_iter.CurrentPosition();
}

Index IndexUtf16ToUtf8(const std::wstring_view& utf16_string, Index utf16_index,
                       const std::string_view& utf8_string) {
  if (utf16_index >= static_cast<Index>(utf16_string.length()))
    return utf8_string.length();

  Index cp_index = 0;
  Utf16Iterator iter{utf16_string};
  while (iter.CurrentPosition() <= utf16_index) {
    iter.Next();
    cp_index++;
  }

  Utf8Iterator result_iter{utf8_string};
  for (Index i = 0; i < cp_index - 1; i++) {
    if (result_iter.Next() == k_code_point_end) break;
  }

  return result_iter.CurrentPosition();
}
}  // namespace cru::platform::win
