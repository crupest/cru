#include "cru/base/Osx.h"
#include "cru/base/StringUtil.h"

namespace cru {
CFWrapper<CFStringRef> ToCFString(std::string_view string) {
  return CFWrapper<CFStringRef>(CFStringCreateWithBytes(
      nullptr, reinterpret_cast<const UInt8*>(string.data()),
      string.size() * sizeof(char), kCFStringEncodingUTF8, false));
}

std::string FromCFStringRef(CFStringRef string) {
  auto length = CFStringGetLength(string);

  std::string result;

  for (int i = 0; i < length; i++) {
    cru::string::Utf8EncodeCodePointAppend(
        CFStringGetCharacterAtIndex(string, i),
        [&result](char c) { result += c; });
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
