#include "cru/base/platform/win/DebugLogTarget.h"

#include "cru/base/StringUtil.h"

namespace cru::platform::win {
void WinDebugLogTarget::Write(::cru::log::LogLevel level, std::string s) {
  CRU_UNUSED(level)

  std::wstring m = string::ToUtf16WString(s);
  ::OutputDebugStringW(reinterpret_cast<const wchar_t*>(m.c_str()));
}
}  // namespace cru::platform::win
