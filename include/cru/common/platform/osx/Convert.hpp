#pragma once
#include "../../PreConfig.hpp"
#ifdef CRU_PLATFORM_OSX

#include "../../String.hpp"

#include <CoreFoundation/CFString.h>

namespace cru::platform::osx {
CFStringRef Convert(const String& string);
String Convert(CFStringRef string);

CFRange Convert(const Range& range);
Range Convert(const CFRange& range);
}  // namespace cru::platform::osx

#endif
