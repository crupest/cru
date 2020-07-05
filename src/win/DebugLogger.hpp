#include "cru/win/WinPreConfig.hpp"

#include "cru/common/Logger.hpp"

namespace cru::platform::win {

class WinDebugLoggerSource : public ::cru::log::ILogSource {
 public:
  WinDebugLoggerSource() = default;

  CRU_DELETE_COPY(WinDebugLoggerSource)
  CRU_DELETE_MOVE(WinDebugLoggerSource)

  ~WinDebugLoggerSource() = default;

  void Write(::cru::log::LogLevel level, const std::u16string& s) override {
    CRU_UNUSED(level)

    ::OutputDebugStringW(reinterpret_cast<const wchar_t*>(s.c_str()));
  }
};
}  // namespace cru::platform::win
