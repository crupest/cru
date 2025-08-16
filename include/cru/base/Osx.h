#pragma once

#ifndef __APPLE__
#error "This header can only be included on OSX platforms."
#endif

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

CFWrapper<CFStringRef> ToCFStringRef(StringView string);
String FromCFStringRef(CFStringRef string);
}  // namespace cru
