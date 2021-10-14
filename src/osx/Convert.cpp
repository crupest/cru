#include "cru/osx/Convert.hpp"

namespace cru::platform::osx {
CFStringRef Convert(const String& string) {
  return CFStringCreateWithBytes(
      nullptr, reinterpret_cast<const UInt8*>(string.data()),
      string.size() * sizeof(std::uint16_t), kCFStringEncodingUTF16, false);
}

String Convert(CFStringRef string) {
  auto d = CFStringCreateExternalRepresentation(nullptr, string,
                                                kCFStringEncodingUTF16, 0);
  auto l = CFDataGetLength(d);

  auto s = String(reinterpret_cast<const char16_t*>(CFDataGetBytePtr(d)),
                  CFDataGetLength(d) / 2);

  CFRelease(d);

  return s;
}

CFRange Convert(const Range& range) {
  return CFRangeMake(range.position, range.count);
}

Range Convert(const CFRange& range) {
  return Range(range.location, range.length);
}
}  // namespace cru::cru::platform::osx
