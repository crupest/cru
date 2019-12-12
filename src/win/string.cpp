#include "cru/win/string.hpp"

#include "cru/win/exception.hpp"

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
}  // namespace cru::platform::win
