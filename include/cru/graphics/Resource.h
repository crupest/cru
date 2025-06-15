#pragma once
#include "Base.h"

namespace cru::graphics {
struct IGraphicsFactory;

struct CRU_PLATFORM_GRAPHICS_API IGraphicsResource : virtual Interface {
  virtual IGraphicsFactory* GetGraphicsFactory() = 0;
};
}  // namespace cru::graphics
