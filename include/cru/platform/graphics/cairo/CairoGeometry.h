#pragma once
#include "CairoResource.h"
#include "cru/platform/graphics/Geometry.h"

#include <cairo/cairo.h>

namespace cru::platform::graphics::cairo {
class CRU_PLATFORM_GRAPHICS_CAIRO_API CairoGeometry : public CairoResource,
                                                      public virtual IGeometry {
 public:
  CairoGeometry(CairoGraphicsFactory* factory, cairo_path_t* cairo_path,
                bool auto_destroy);
  ~CairoGeometry();

  bool FillContains(const Point& point) override;
  Rect GetBounds() override;
  std::unique_ptr<IGeometry> Transform(const Matrix& matrix) override;
  std::unique_ptr<IGeometry> CreateStrokeGeometry(float width) override;

  cairo_path_t* GetCairoPath() const { return cairo_path_; }

 private:
  cairo_path_t* cairo_path_;
  bool auto_destroy_;
};

class CRU_PLATFORM_GRAPHICS_CAIRO_API CairoGeometryBuilder
    : public CairoResource,
      public virtual IGeometryBuilder {};
}  // namespace cru::platform::graphics::cairo
