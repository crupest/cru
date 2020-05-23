#include "cru/win/WinPreConfig.hpp"

#include "cru/common/Logger.hpp"
#include "cru/win/String.hpp"

namespace cru::platform::win {

class WinDebugLoggerSource : public ::cru::log::ILogSource {
 public:
  WinDebugLoggerSource() = default;

  CRU_DELETE_COPY(WinDebugLoggerSource)
  CRU_DELETE_MOVE(WinDebugLoggerSource)

  ~WinDebugLoggerSource() = default;

  void Write(::cru::log::LogLevel level, const std::string_view& s) override {
    CRU_UNUSED(level)

    const std::wstring&& ws = ToUtf16String(s);
    ::OutputDebugStringW(ws.data());
  }
};
}  // namespace cru::platform::win
