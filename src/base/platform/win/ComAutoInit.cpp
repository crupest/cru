#include "cru/base/platform/win/ComAutoInit.h"
#include "cru/base/platform/win/Exception.h"

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
