#pragma once
#include "../../PreConfig.h"

#ifdef CRU_PLATFORM_WINDOWS

#include "../../io/Stream.h"

#include <objidlbase.h>

namespace cru::platform::win {
CRU_BASE_API IStream* ConvertStreamToComStream(io::Stream* stream);
}

#endif
