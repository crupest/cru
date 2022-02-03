#include "cru/common/platform/win/ComAutoInit.hpp"
#include "cru/common/platform/win/Exception.hpp"

#include <combaseapi.h>

namespace cru::platform::win {
ComAutoInit::ComAutoInit() {
  const auto hresult = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  if (FAILED(hresult)) {
    throw HResultError(hresult, "Failed to call CoInitializeEx.");
  }
}

ComAutoInit::~ComAutoInit() { ::CoUninitialize(); }
}  // namespace cru::platform::win
