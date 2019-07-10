#include "cru/win/win_pre_config.hpp"

#include "cru/platform/heap_debug.hpp"

#include <crtdbg.h>

namespace cru::platform {
void SetupHeapDebug() {
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}
}
