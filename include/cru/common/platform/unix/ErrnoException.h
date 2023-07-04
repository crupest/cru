#pragma once

#include "../../PreConfig.h"

#ifdef CRU_PLATFORM_UNIX

#include "../Exception.h"

namespace cru::platform::unix {
// Moved to Exception.h
using ErrnoException = cru::ErrnoException;
}  // namespace cru::platform::unix

#endif
