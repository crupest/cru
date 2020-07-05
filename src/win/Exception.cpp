#include "cru/win/Exception.hpp"

#include <fmt/format.h>
#include <optional>

namespace cru::platform::win {

inline std::string HResultMakeMessage(
    HRESULT h_result, std::optional<std::string_view> message) {
  if (message.has_value())
    return fmt::format(FMT_STRING("HRESULT: {:#08x}. Message: {}"), h_result,
                       *message);
  else
    return fmt::format(FMT_STRING("HRESULT: {:#08x}."), h_result);
}

HResultError::HResultError(HRESULT h_result)
    : PlatformException(HResultMakeMessage(h_result, std::nullopt)),
      h_result_(h_result) {}

HResultError::HResultError(HRESULT h_result,
                           std::string_view additional_message)
    : PlatformException(HResultMakeMessage(h_result, additional_message)),
      h_result_(h_result) {}

inline std::string Win32MakeMessage(DWORD error_code,
                                       std::string_view message) {
  return fmt::format("Last error code: {:#04x}.\nMessage: {}\n", error_code,
                     message);
}

Win32Error::Win32Error(std::string_view message)
    : Win32Error(::GetLastError(), message) {}

Win32Error::Win32Error(DWORD error_code, std::string_view message)
    : PlatformException(Win32MakeMessage(error_code, message)),
      error_code_(error_code) {}
}  // namespace cru::platform::win
