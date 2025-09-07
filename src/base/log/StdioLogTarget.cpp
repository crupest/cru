#include "cru/base/log/StdioLogTarget.h"

#ifdef _WIN32
#include "cru/base/String.h"
#endif

#include <iostream>

namespace cru::log {
StdioLogTarget::StdioLogTarget() {}

StdioLogTarget::~StdioLogTarget() {}

void StdioLogTarget::Write(log::LogLevel level, std::string message) {
#ifdef CRU_PLATFORM_WINDOWS
  String s = String::FromUtf8(message);
  if (level == log::LogLevel::Error) {
    std::wcerr.write(reinterpret_cast<const wchar_t*>(s.data()), s.size());
  } else {
    std::wcout.write(reinterpret_cast<const wchar_t*>(s.data()), s.size());
  }
#else
  if (level == log::LogLevel::Error) {
    std::cerr << message;
  } else {
    std::cout << message;
  }
#endif
}
}  // namespace cru::log
