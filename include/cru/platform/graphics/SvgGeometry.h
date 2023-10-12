#pragma once

#include "Geometry.h"

namespace cru::platform::graphics {
class CRU_PLATFORM_GRAPHICS_API SvgGeometryBuilder
    : public Object,
      public virtual IGeometryBuilder {
 public:
  SvgGeometryBuilder();

  ~SvgGeometryBuilder() override;

 private:
  String current_;
};
}  // namespace cru::platform::graphics
