#include "cru/base/log/StdioLogWriter.h"

#include <iostream>
#include "cru/base/StringUtil.h"

namespace cru::log {
StdioLogWriter::StdioLogWriter() {}

StdioLogWriter::~StdioLogWriter() {}

void StdioLogWriter::Write(const LogInfo& log_info, std::string log_str) {
#ifdef _WIN32
  auto s = string::ToUtf16WString(log_str);
  if (log_info.level == log::LogLevel::Error) {
    std::wcerr << s << std::endl;
  } else {
    std::wcout << s << std::endl;
  }
#else
  if (level == log::LogLevel::Error) {
    std::cerr << log_str << std::endl;
  } else {
    std::cout << log_str << std::endl;
  }
#endif
}
}  // namespace cru::log
