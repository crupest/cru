#include "cru/base/Osx.h"

namespace cru {
CFWrapper<CFStringRef> ToCFString(StringView string) {
  return CFWrapper<CFStringRef>(CFStringCreateWithBytes(
      nullptr, reinterpret_cast<const UInt8*>(string.data()),
      string.size() * sizeof(std::uint16_t), kCFStringEncodingUTF16, false));
}

String FromCFStringRef(CFStringRef string) {
  auto length = CFStringGetLength(string);

  String result;

  for (int i = 0; i < length; i++) {
    result.AppendCodePoint(CFStringGetCharacterAtIndex(string, i));
  }

  return result;
}

CFRange ToCFRange(const Range& range) {
  return CFRangeMake(range.position, range.count);
}

Range FromCFRange(const CFRange& range) {
  return Range(range.location, range.length);
}
}  // namespace cru
