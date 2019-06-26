#include "cru/win/graph/direct/exception.hpp"

#include "cru/common/format.hpp"

namespace cru::platform::graph::win::direct {
using util::Format;

inline std::string HResultMakeMessage(HRESULT h_result,
                                      const std::string_view* message) {
  char buffer[10];
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
}  // namespace cru::platform::graph::win::direct
