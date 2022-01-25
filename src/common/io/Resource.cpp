#include "cru/common/io/Resource.hpp"
#include "cru/common/Exception.hpp"

#if defined(CRU_PLATFORM_OSX)
#include <CoreFoundation/CoreFoundation.h>
#elif defined(CRU_PLATFORM_WINDOWS)
#endif

namespace cru::io {
std::unique_ptr<Stream> CreateStreamFromResourcePath(const String& path) {
#if defined(CRU_PLATFORM_OSX)
  // CFBundleRef main_bundle = CFBundleGetMainBundle();
  throw Exception(u"Not implemented.");
#elif defined(CRU_PLATFORM_WINDOWS)
  throw Exception(u"Not implemented.");
#else
  throw Exception(u"Not implemented.");
#endif
}
}  // namespace cru::io
