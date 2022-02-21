#include "cru/common/log/StdioLogTarget.h"

#include <iostream>

namespace cru::log {
StdioLogTarget::StdioLogTarget() {}

StdioLogTarget::~StdioLogTarget() {}

void StdioLogTarget::Write(log::LogLevel level, StringView s) {
#ifdef CRU_PLATFORM_WINDOWS
  if (level == log::LogLevel::Error) {
    std::wcerr.write(reinterpret_cast<const wchar_t*>(s.data()), s.size());
  } else {
    std::wcout.write(reinterpret_cast<const wchar_t*>(s.data()), s.size());
  }
#else
  std::string m = s.ToUtf8();

  if (level == log::LogLevel::Error) {
    std::cerr << m;
  } else {
    std::cout << m;
  }
#endif
}
}  // namespace cru::log
