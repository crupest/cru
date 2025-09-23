#include "cru/base/io/Resource.h"
#include "cru/base/Base.h"
#include "cru/base/Exception.h"

#if defined(CRU_PLATFORM_OSX)
#include <CoreFoundation/CoreFoundation.h>
#elif defined(CRU_PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(__linux)
#include <fstream>
#include <sstream>
#endif

#include <filesystem>

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
#elif defined(_WIN32) || defined(__linux)
#if defined(_WIN32)
  wchar_t buffer[MAX_PATH];
  DWORD size = ::GetModuleFileNameW(nullptr, buffer, MAX_PATH);
  std::filesystem::path module_path(buffer, buffer + size);
#else  // linux
  std::ifstream file("/proc/self/cmdline");
  std::stringstream buffer;
  buffer << file.rdbuf();
  auto str = buffer.str();
  std::filesystem::path module_path(str.substr(0, str.find('\0')));
#endif
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

#endif

  NotImplemented();
}
}  // namespace cru::io
