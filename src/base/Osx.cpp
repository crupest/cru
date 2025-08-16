#include "cru/base/Osx.h"

namespace cru {
CFWrapper<CFStringRef> ToCFStringRef(StringView string) {
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
}  // namespace cru
