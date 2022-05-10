#pragma once

#include "../Factory.h"
#include "CairoResource.h"

namespace cru::platform::graphics::cairo {
class CRU_PLATFORM_GRAPHICS_CAIRO_API CairoGraphicsFactory
    : public CairoResource,
      public virtual IGraphicsFactory {
 public:
  CairoGraphicsFactory();
  ~CairoGraphicsFactory() override;

 public:
};
}  // namespace cru::platform::graphics::cairo
