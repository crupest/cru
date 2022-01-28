#pragma once
#include "Base.hpp"

#include "cru/common/Logger.hpp"

namespace cru::platform::win {
class CRU_WIN_API WinStdOutLoggerSource : public ::cru::log::ILogSource {
 public:
  WinStdOutLoggerSource() = default;

  CRU_DELETE_COPY(WinStdOutLoggerSource)
  CRU_DELETE_MOVE(WinStdOutLoggerSource)

  ~WinStdOutLoggerSource() = default;

  void Write(::cru::log::LogLevel level, StringView s) override {
    CRU_UNUSED(level)

    String m = s.ToString();
    std::fputws(reinterpret_cast<const wchar_t*>(m.c_str()), stdout);
  }
};
}  // namespace cru::platform::win
