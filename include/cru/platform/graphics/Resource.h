#pragma once
#include "Base.h"

#include <cru/platform/Resource.h>

namespace cru::platform::graphics {
struct IGraphicsFactory;

struct CRU_PLATFORM_GRAPHICS_API IGraphicsResource : virtual IPlatformResource {
  virtual IGraphicsFactory* GetGraphicsFactory() = 0;
};
}  // namespace cru::platform::graphics
