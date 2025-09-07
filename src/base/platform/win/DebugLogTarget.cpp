#include "cru/base/platform/win/DebugLogTarget.h"

#include "cru/base/String.h"

namespace cru::platform::win {
void WinDebugLogTarget::Write(::cru::log::LogLevel level, std::string s) {
  CRU_UNUSED(level)

  String m = String::FromUtf8(s);
  ::OutputDebugStringW(reinterpret_cast<const wchar_t*>(m.c_str()));
}
}  // namespace cru::platform::win
