#pragma once

#ifndef _WIN32
#error "This file can only be included on Windows."
#endif

#include "Base.h"
#include "../../log/Logger.h"

namespace cru::platform::win {
class CRU_BASE_API WinDebugLogWriter : public ::cru::log::ILogWriter {
 public:
  void Write(const ::cru::log::LogInfo& log_info, std::string log_str) override;
};
}  // namespace cru::platform::win
