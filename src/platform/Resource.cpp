#include "cru/platform/Resource.h"

namespace cru::platform {
std::string IPlatformResource::GetPlatformIdUtf8() const {
  return GetPlatformId().ToUtf8();
}
}  // namespace cru::platform
