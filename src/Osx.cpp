#include <cru/Osx.h>

#include <CoreFoundation/CoreFoundation.h>

namespace cru::osx {
CFWrapper<CFStringRef> ToCF(StringView string) {
  return CFWrapper<CFStringRef>(CFStringCreateWithBytes(
      nullptr, reinterpret_cast<const UInt8*>(string.data()),
      string.size() * sizeof(std::uint16_t), kCFStringEncodingUTF16, false));
}

CFWrapper<CFStringRef> ToCF(const String& string) {
  return ToCF(StringView(string));
}

String FromCF(CFStringRef string) {
  auto length = CFStringGetLength(string);

  String result;

  for (int i = 0; i < length; i++) {
    result.AppendCodePoint(CFStringGetCharacterAtIndex(string, i));
  }

  return result;
}
}

