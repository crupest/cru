#pragma once

#include "../../PreConfig.h"
#ifdef CRU_PLATFORM_WINDOWS

#include "WinPreConfig.h"

#include "../../log/Logger.h"

namespace cru::platform::win {

class CRU_BASE_API WinDebugLogTarget : public ::cru::log::ILogTarget {
 public:
  WinDebugLogTarget() = default;

  CRU_DELETE_COPY(WinDebugLogTarget)
  CRU_DELETE_MOVE(WinDebugLogTarget)

  ~WinDebugLogTarget() = default;

  void Write(::cru::log::LogLevel level, StringView s) override;
};
}  // namespace cru::platform::win

#endif
