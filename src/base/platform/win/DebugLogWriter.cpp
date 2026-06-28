#include "cru/base/platform/win/DebugLogWriter.h"

#include "cru/base/StringUtil.h"

namespace cru::platform::win {
void WinDebugLogWriter::Write(const ::cru::log::LogInfo& log_info,
                              std::string log_str) {
  CRU_UNUSED(log_info)

  std::wstring m = string::ToUtf16WString(log_str);
  ::OutputDebugStringW(m.c_str());
}
}  // namespace cru::platform::win
