#pragma once

#ifndef __APPLE__
#error "This header can only be included on OSX."
#endif

#include "./String.h"
#include "./Range.h"

#include <CoreFoundation/CoreFoundation.h>

namespace cru::osx {
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

inline CFRange ToCF(Range range) {
  return CFRangeMake(range.position, range.count);
}

inline Range FromCF(const CFRange& range) {
  return Range(range.location, range.length);
}

CFWrapper<CFStringRef> ToCF(const String& string);
CFWrapper<CFStringRef> ToCF(StringView string);
String FromCF(CFStringRef string);

}  // namespace cru
