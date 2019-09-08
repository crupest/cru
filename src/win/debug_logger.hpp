#include "cru/win/win_pre_config.hpp"

#include "cru/common/logger.hpp"

namespace cru::platform::win {

class WinDebugLoggerSource : public ::cru::log::ILoggerSource {
 public:
  WinDebugLoggerSource() = default;

  CRU_DELETE_COPY(WinDebugLoggerSource)
  CRU_DELETE_MOVE(WinDebugLoggerSource)

  ~WinDebugLoggerSource() = default;

  void Write(::cru::log::LogLevel level, const std::wstring_view& s) override {
    ::OutputDebugStringW(s.data());
  }
};
}  // namespace cru::platform::win
