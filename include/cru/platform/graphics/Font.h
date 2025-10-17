#pragma once
#include "Resource.h"

#include <string>

namespace cru::platform::graphics {
struct CRU_PLATFORM_GRAPHICS_API IFont : virtual IGraphicsResource {
  virtual std::string GetFontName() = 0;
  virtual float GetFontSize() = 0;
};
}  // namespace cru::platform::graphics
