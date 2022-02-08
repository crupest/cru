#pragma once
#include "Resource.h"

namespace cru::platform::graphics {
struct CRU_PLATFORM_GRAPHICS_API IImage : public virtual IGraphicsResource {
  virtual float GetWidth() = 0;
  virtual float GetHeight() = 0;
  virtual std::unique_ptr<IImage> CreateWithRect(const Rect& rect) = 0;
};
}  // namespace cru::platform::graphics
