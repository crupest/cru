#pragma once

#ifdef CRU_PLATFORM_WINDOWS

#include "WinPreConfig.h"
#include <cru/Base.h>

namespace cru::platform::win {
class CRU_BASE_API ComAutoInit {
 public:
  ComAutoInit();


  ~ComAutoInit();
};
}  // namespace cru::platform::win

#endif
