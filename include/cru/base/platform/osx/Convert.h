#pragma once

#ifndef CRU_PLATFORM_OSX
#error "This file can only be included on osx."
#endif

#include "../../String.h"

#include <CoreFoundation/CoreFoundation.h>

namespace cru::platform::osx {
CFStringRef Convert(const String& string);
String Convert(CFStringRef string);

CFRange Convert(const Range& range);
Range Convert(const CFRange& range);
}  // namespace cru::platform::osx
