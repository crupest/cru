#pragma once

#include "../../PreConfig.hpp"
#ifdef CRU_PLATFORM_WINDOWS

#include "WinPreConfig.hpp"
#include "cru/common/Base.hpp"

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
