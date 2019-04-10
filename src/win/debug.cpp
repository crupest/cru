#include "cru/win/win_pre_config.hpp"

#include "cru/platform/debug.hpp"

namespace cru::platform {
void DebugMessage(const std::wstring_view& message) {
  ::OutputDebugStringW(message.data());
}
}  // namespace cru::platform
