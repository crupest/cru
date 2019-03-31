#include "cru/platform/win/win_pre_config.hpp"

#include "cru/platform/debug.hpp"


namespace cru::debug {
void DebugMessage(const std::wstring_view& message) {
  ::OutputDebugStringW(message.data());
}
}  // namespace cru::debug
