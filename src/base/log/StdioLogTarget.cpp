#include "cru/base/log/StdioLogTarget.h"

#include <iostream>
#include "cru/base/StringUtil.h"

namespace cru::log {
StdioLogTarget::StdioLogTarget() {}

StdioLogTarget::~StdioLogTarget() {}

void StdioLogTarget::Write(log::LogLevel level, std::string message) {
#ifdef _WIN32
  auto s = string::ToUtf16WString(message);
  if (level == log::LogLevel::Error) {
    std::wcerr << s << std::endl;
  } else {
    std::wcout << s << std::endl;
  }
#else
  if (level == log::LogLevel::Error) {
    std::cerr << message << std::endl;
  } else {
    std::cout << message << std::endl;
  }
#endif
}
}  // namespace cru::log
