#include "cru/common/platform/win/DebugLogTarget.h"

namespace cru::platform::win {
void WinDebugLogTarget::Write(::cru::log::LogLevel level, StringView s) {
  CRU_UNUSED(level)

  String m = s.ToString();
  ::OutputDebugStringW(reinterpret_cast<const wchar_t*>(m.c_str()));
}
}  // namespace cru::platform::win
