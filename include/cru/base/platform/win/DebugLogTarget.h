#pragma once

#ifndef _WIN32
#error "This file can only be used on Windows."
#endif

#include "WinPreConfig.h"

#include "../../log/Logger.h"

namespace cru::platform::win {

class CRU_BASE_API WinDebugLogTarget : public ::cru::log::ILogTarget {
 public:
  void Write(::cru::log::LogLevel level, std::string s) override;
};
}  // namespace cru::platform::win
