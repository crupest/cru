#pragma once

#ifndef __APPLE__
#error "This header can only be included on OSX platforms."
#endif

#include "../../Guard.h"
#include "../../Range.h"

#include <CoreFoundation/CoreFoundation.h>
#include <string>
#include <string_view>

namespace cru {
namespace details {
template <typename CFClassRef>
struct CFWrapperDeleter {
  void operator()(CFClassRef ref) const noexcept { CFRelease(ref); }
};
}  // namespace details

template <typename CFClassRef>
using CFWrapper =
    AutoDestruct<CFClassRef, details::CFWrapperDeleter<CFClassRef>>;

CFWrapper<CFStringRef> ToCFString(std::string_view string);
std::string FromCFStringRef(CFStringRef string);

CFRange ToCFRange(const Range& range);
Range FromCFRange(const CFRange& range);
}  // namespace cru
