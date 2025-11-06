#pragma once

#ifndef _WIN32
#error "This file can only be included on Windows."
#endif

#include "Base.h"

namespace cru::platform::win {
class CRU_BASE_API ComAutoInit {
 public:
  ComAutoInit();

  CRU_DELETE_COPY(ComAutoInit)
  CRU_DELETE_MOVE(ComAutoInit)

  ~ComAutoInit();
};
}  // namespace cru::platform::win
