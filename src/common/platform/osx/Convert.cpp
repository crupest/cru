#include "cru/common/platform/osx/Convert.h"

namespace cru::platform::osx {
CFStringRef Convert(const String& string) {
  return CFStringCreateWithBytes(
      nullptr, reinterpret_cast<const UInt8*>(string.data()),
      string.size() * sizeof(std::uint16_t), kCFStringEncodingUTF16, false);
}

String Convert(CFStringRef string) {
  auto length = CFStringGetLength(string);

  String result;

  for (int i = 0; i < length; i++) {
    result.AppendCodePoint(CFStringGetCharacterAtIndex(string, i));
  }

  return result;
}

CFRange Convert(const Range& range) {
  return CFRangeMake(range.position, range.count);
}

Range Convert(const CFRange& range) {
  return Range(range.location, range.length);
}
}  // namespace cru::platform::osx
