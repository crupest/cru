#include "cru/base/platform/win/Exception.h"

#include <format>
#include <optional>

namespace cru::platform::win {

inline std::string HResultMakeMessage(HRESULT h_result,
                                      std::optional<std::string_view> message) {
  if (message.has_value())
    return std::format("HRESULT: {}. Message: {}", h_result, *message);
  else
    return std::format("HRESULT: {}.", h_result);
}

HResultError::HResultError(HRESULT h_result)
    : Exception(HResultMakeMessage(h_result, std::nullopt)),
      h_result_(h_result) {}

HResultError::HResultError(HRESULT h_result,
                           std::string_view additional_message)
    : Exception(HResultMakeMessage(h_result, additional_message)),
      h_result_(h_result) {}

inline std::string Win32MakeMessage(DWORD error_code,
                                    std::string_view message) {
  return std::format("Last error code: {}.\nMessage: {}\n", error_code,
                     message);
}

Win32Error::Win32Error(std::string_view message)
    : Win32Error(::GetLastError(), message) {}

Win32Error::Win32Error(DWORD error_code, std::string_view message)
    : Exception(Win32MakeMessage(error_code, message)),
      error_code_(error_code) {}
}  // namespace cru::platform::win
