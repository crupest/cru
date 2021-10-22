#include "cru/osx/Convert.hpp"
#include <string>

#include "cru/common/StringUtil.hpp"

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
    std::u16string s;
    Utf16EncodeCodePointAppend(CFStringGetCharacterAtIndex(string, i), s);
    result.append(s.data(), s.size());
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
