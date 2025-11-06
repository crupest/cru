#pragma once

#ifndef _WIN32
#error "This file can only be included on Windows."
#endif

#include "../../io/Stream.h"

#include <objidlbase.h>

namespace cru::platform::win {
CRU_BASE_API IStream* ConvertStreamToComStream(io::Stream* stream);
}
