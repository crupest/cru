#include "cru/platform/string_util.hpp"

#include "cru/platform/win/exception.hpp"

namespace cru::platform::util {
std::string ToUtf8String(const std::wstring_view& string) {
  if (string.empty()) return std::string();

  const auto length = ::WideCharToMultiByte(CP_UTF8, 0, string.data(), -1,
                                            nullptr, 0, nullptr, nullptr);
  std::string result;
  result.resize(length);
  if (::WideCharToMultiByte(CP_UTF8, 0, string.data(), -1, result.data(),
                            static_cast<int>(result.size()), nullptr,
                            nullptr) == 0)
    throw win::Win32Error(::GetLastError(),
                          "Failed to convert wide string to UTF-8.");
  return result;
}
}  // namespace cru::platform::util
