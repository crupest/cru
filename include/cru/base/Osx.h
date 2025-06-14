#pragma once

#ifndef CRU_PLATFORM_OSX
#error "This header can only be included on OSX."
#endif

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
}  // namespace cru
