#include "cru/win/exception.hpp"

#include "cru/common/format.hpp"

namespace cru::platform::win {
using util::Format;

inline std::string HResultMakeMessage(HRESULT h_result,
                                      const std::string_view* message) {
  char buffer[20];
  sprintf_s(buffer, "%#08x", h_result);

  if (message)
    return Format(
        "An HResultError is thrown. HRESULT: {}.\nAdditional message: {}\n",
        buffer, *message);
  else
    return Format("An HResultError is thrown. HRESULT: {}.\n", buffer);
}

HResultError::HResultError(HRESULT h_result)
    : PlatformException(HResultMakeMessage(h_result, nullptr)),
      h_result_(h_result) {}

HResultError::HResultError(HRESULT h_result,
                           const std::string_view& additional_message)
    : PlatformException(HResultMakeMessage(h_result, &additional_message)),
      h_result_(h_result) {}

inline std::string Win32MakeMessage(DWORD error_code,
                                    const std::string_view* message) {
  char buffer[20];
  sprintf_s(buffer, "%#04x", error_code);

  if (message)
    return Format("Last error code: {}.\nAdditional message: {}\n", buffer,
                  *message);
  else
    return Format("Last error code: {}.\n", buffer);
}

Win32Error::Win32Error(DWORD error_code)
    : PlatformException(Win32MakeMessage(error_code, nullptr)),
      error_code_(error_code) {}

Win32Error::Win32Error(DWORD error_code,
                       const std::string_view& additional_message)
    : PlatformException(Win32MakeMessage(error_code, &additional_message)),
      error_code_(error_code) {}
}  // namespace cru::win
