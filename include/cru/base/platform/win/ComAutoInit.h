#pragma once

#ifdef CRU_PLATFORM_WINDOWS

#include "WinPreConfig.h"
#include "cru/base/Base.h"

namespace cru::platform::win {
class CRU_BASE_API ComAutoInit {
 public:
  ComAutoInit();

  CRU_DELETE_COPY(ComAutoInit)
  CRU_DELETE_MOVE(ComAutoInit)

  ~ComAutoInit();
};
}  // namespace cru::platform::win

#endif
