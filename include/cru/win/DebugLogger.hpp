#pragma once
#include "Base.hpp"

#include "cru/common/Logger.hpp"

namespace cru::platform::win {

class CRU_WIN_API WinDebugLoggerSource : public ::cru::log::ILogSource {
 public:
  WinDebugLoggerSource() = default;

  CRU_DELETE_COPY(WinDebugLoggerSource)
  CRU_DELETE_MOVE(WinDebugLoggerSource)

  ~WinDebugLoggerSource() = default;

  void Write(::cru::log::LogLevel level, StringView s) override {
    CRU_UNUSED(level)

    String m = s.ToString();
    ::OutputDebugStringW(reinterpret_cast<const wchar_t*>(m.c_str()));
  }
};
}  // namespace cru::platform::win
