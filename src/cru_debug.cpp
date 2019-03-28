#include "cru_debug.hpp"

#include <Windows.h>

namespace cru::debug {
#ifdef CRU_DEBUG
void DebugMessage(const StringView& message) {
  ::OutputDebugStringW(message.data());
}
#endif
}  // namespace cru::debug
