#include "exception.hpp"

#include "util/format.hpp"

namespace cru {
using util::Format;

inline std::string HResultMakeMessage(
    HRESULT h_result, std::optional<MultiByteStringView> message) {
  char buffer[10];
  sprintf_s(buffer, "%#08x", h_result);

  if (message.has_value())
    return Format(
        "An HResultError is thrown. HRESULT: {}.\nAdditional message: {}\n",
        buffer, message.value());
  else
    return Format("An HResultError is thrown. HRESULT: {}.\n", buffer);
}

HResultError::HResultError(
    HRESULT h_result, std::optional<MultiByteStringView> additional_message)
    : runtime_error(HResultMakeMessage(h_result, std::nullopt)),
      h_result_(h_result) {}

inline std::string Win32MakeMessage(
    DWORD error_code, std::optional<MultiByteStringView> message) {
  char buffer[10];
  sprintf_s(buffer, "%#04x", error_code);

  if (message.has_value())
    return Format("Last error code: {}.\nAdditional message: {}\n", buffer,
                  message.value());
  else
    return Format("Last error code: {}.\n", buffer);
}

Win32Error::Win32Error(DWORD error_code,
                       std::optional<MultiByteStringView> additional_message)
    : runtime_error(Win32MakeMessage(error_code, std::nullopt)),
      error_code_(error_code) {}
}  // namespace cru
