#include "cru/base/io/Resource.h"
#include "cru/base/Exception.h"
#include "cru/base/log/Logger.h"

#if defined(CRU_PLATFORM_OSX)
#include <CoreFoundation/CoreFoundation.h>
#elif defined(CRU_PLATFORM_WINDOWS)
#include <Windows.h>
#endif

#include <filesystem>

namespace cru::io {
std::filesystem::path GetResourceDir() {
  constexpr auto kLogTag = u"GetResourceDir";

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
  wchar_t buffer[MAX_PATH];
  DWORD size = ::GetModuleFileNameW(nullptr, buffer, MAX_PATH);
  std::filesystem::path module_path(buffer, buffer + size);
  auto p = module_path;
  while (p.has_parent_path()) {
    p = p.parent_path();
    auto resource_dir_path = p / "assets";
    if (std::filesystem::exists(resource_dir_path) &&
        std::filesystem::is_directory(resource_dir_path)) {
      return resource_dir_path;
    }
  }

  throw Exception(u"Failed to find resource directory.");
#else
  throw Exception(u"Not implemented.");
#endif
}
}  // namespace cru::io
