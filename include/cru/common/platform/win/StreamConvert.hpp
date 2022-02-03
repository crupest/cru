#pragma once
#include "../../PreConfig.hpp"

#ifdef CRU_PLATFORM_WINDOWS

#include "../../io/Stream.hpp"

#include <objidlbase.h>

namespace cru::platform::win {
CRU_BASE_API IStream* ConvertStreamToComStream(io::Stream* stream);
}

#endif
