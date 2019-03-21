#include "cru_debug.hpp"

#include "system_headers.hpp"

namespace cru::debug {
#ifdef CRU_DEBUG
void DebugMessage(const StringView& message) {
  ::OutputDebugStringW(message.data());
}
#endif
}  // namespace cru::debug
