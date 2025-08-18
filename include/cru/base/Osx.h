#pragma once

// TODO: Move This file to platform dir.

#ifndef __APPLE__
#error "This header can only be included on OSX platforms."
#endif

#include "Range.h"
#include "String.h"

#include <CoreFoundation/CoreFoundation.h>

namespace cru {
template <typename CFClassRef>
class CFWrapper {
 public:
  CFClassRef ref;

  explicit CFWrapper(CFClassRef ref) { this->ref = ref; }
  ~CFWrapper() {
    if (this->ref) CFRelease(this->ref);
  }

  CFClassRef* release() {
    auto ref = this->ref;
    this->ref = nullptr;
    return ref;
  }
};

CFWrapper<CFStringRef> ToCFString(StringView string);
String FromCFStringRef(CFStringRef string);

CFRange ToCFRange(const Range& range);
Range FromCFRange(const CFRange& range);
}  // namespace cru
