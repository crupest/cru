#include "cru/common/Base.hpp"
#include "cru/win/WinPreConfig.hpp"

#include "cru/common/Logger.hpp"

#include <cwchar>

namespace cru::platform::win {
class WinStdOutLoggerSource : public ::cru::log::ILogSource {
 public:
  WinStdOutLoggerSource() = default;

  CRU_DELETE_COPY(WinStdOutLoggerSource)
  CRU_DELETE_MOVE(WinStdOutLoggerSource)

  ~WinStdOutLoggerSource() = default;

  void Write(::cru::log::LogLevel level, const std::u16string& s) override {
    CRU_UNUSED(level)

    std::fputws(reinterpret_cast<const wchar_t*>(s.c_str()), stdout);
  }
};
}  // namespace cru::platform::win
