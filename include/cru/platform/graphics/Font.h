#pragma once
#include "Resource.h"

namespace cru::platform::graphics {
struct CRU_PLATFORM_GRAPHICS_API IFont : virtual IGraphicsResource {
  virtual String GetFontName() = 0;
  virtual float GetFontSize() = 0;
};
}  // namespace cru::platform::graphics
