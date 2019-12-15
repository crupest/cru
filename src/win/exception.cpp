#include "cru/win/exception.hpp"

#include "cru/common/format.hpp"

namespace cru::platform::win {
using util::Format;

inline std::string HResultMakeMessage(HRESULT h_result,
                                      const std::string_view* message) {
  char buffer[20];
  sprintf_s(buffer, "%#08x", h_result);

  if (message)
    return Format("HRESULT: {}.\nMessage: {}\n", buffer, *message);
  else
    return Format("HRESULT: {}.\n", buffer);
}

HResultError::HResultError(HRESULT h_result)
    : PlatformException(HResultMakeMessage(h_result, nullptr)),
      h_result_(h_result) {}

HResultError::HResultError(HRESULT h_result,
                           const std::string_view& additional_message)
    : PlatformException(HResultMakeMessage(h_result, &additional_message)),
      h_result_(h_result) {}

inline std::string Win32MakeMessage(DWORD error_code,
                                    const std::string_view& message) {
  char buffer[20];
  sprintf_s(buffer, "%#04x", error_code);

  return Format("Last error code: {}.\nMessage: {}\n", buffer, message);
}

Win32Error::Win32Error(const std::string_view& message)
    : Win32Error(::GetLastError(), message) {}

Win32Error::Win32Error(DWORD error_code, const std::string_view& message)
    : PlatformException(Win32MakeMessage(error_code, message)),
      error_code_(error_code) {}
}  // namespace cru::platform::win
