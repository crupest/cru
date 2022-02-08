#pragma once
#include "../../PreConfig.h"
#ifdef CRU_PLATFORM_OSX

#include "../../String.h"

#include <CoreFoundation/CoreFoundation.h>

namespace cru::platform::osx {
CFStringRef Convert(const String& string);
String Convert(CFStringRef string);

CFRange Convert(const Range& range);
Range Convert(const CFRange& range);
}  // namespace cru::platform::osx

#endif
