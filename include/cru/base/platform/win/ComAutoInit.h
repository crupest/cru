#pragma once

#ifndef _WIN32
#error "This file can only be included on Windows."
#endif

#include "Base.h"

namespace cru::platform::win {
class CRU_BASE_API ComAutoInit : public Object {
 public:
  ComAutoInit();
  ~ComAutoInit();
};
}  // namespace cru::platform::win
