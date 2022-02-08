#include "cru/common/io/Resource.h"
#include "cru/common/Exception.h"

#if defined(CRU_PLATFORM_OSX)
#include <CoreFoundation/CoreFoundation.h>
#elif defined(CRU_PLATFORM_WINDOWS)
#endif

namespace cru::io {
std::filesystem::path GetResourceDir() {
#if defined(CRU_PLATFORM_OSX)
  CFBundleRef main_bundle = CFBundleGetMainBundle();
  CFURLRef bundle_url = CFBundleCopyBundleURL(main_bundle);
  CFStringRef cf_string_ref =
      CFURLCopyFileSystemPath(bundle_url, kCFURLPOSIXPathStyle);
  std::filesystem::path bundle_path(
      CFStringGetCStringPtr(cf_string_ref, kCFStringEncodingUTF8));

  CFRelease(bundle_url);
  CFRelease(cf_string_ref);

  return bundle_path / "Contents/Resources";
#elif defined(CRU_PLATFORM_WINDOWS)
  throw Exception(u"Not implemented.");
#else
  throw Exception(u"Not implemented.");
#endif
}
}  // namespace cru::io
