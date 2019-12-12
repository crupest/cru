#include "cru/win/win_pre_config.hpp"

#include "cru/common/logger.hpp"

namespace cru::platform::win {

class WinDebugLoggerSource : public ::cru::log::ILogSource {
 public:
  WinDebugLoggerSource() = default;

  CRU_DELETE_COPY(WinDebugLoggerSource)
  CRU_DELETE_MOVE(WinDebugLoggerSource)

  ~WinDebugLoggerSource() = default;

  void Write(::cru::log::LogLevel level, const std::string_view& s) override {
    ::OutputDebugStringA(s.data());
  }
};
}  // namespace cru::platform::win
