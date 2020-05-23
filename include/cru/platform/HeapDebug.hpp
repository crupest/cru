#pragma once
#include "cru/common/PreConfig.hpp"

namespace cru::platform {
// Setup the heap debug function. Currently I only use this on Windows...
void SetupHeapDebug();
}  // namespace cru::platform
