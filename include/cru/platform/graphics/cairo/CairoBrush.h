#pragma once
#include "CairoResource.h"
#include "cru/platform/graphics/Brush.h"

#include <cairo/cairo.h>

namespace cru::platform::graphics::cairo {
class CRU_PLATFORM_GRAPHICS_CAIRO_API CairoBrush : public CairoResource,
                                                   public virtual IBrush {
 public:
  explicit CairoBrush(CairoGraphicsFactory* factory);
  ~CairoBrush() override;

  virtual cairo_pattern_t* GetCairoPattern() = 0;
};

class CRU_PLATFORM_GRAPHICS_CAIRO_API CairoSolidColorBrush
    : public CairoBrush,
      public virtual ISolidColorBrush {
 public:
  explicit CairoSolidColorBrush(CairoGraphicsFactory* factory);
  ~CairoSolidColorBrush() override;

 public:
  Color GetColor() override;
  void SetColor(const Color& color) override;

  cairo_pattern_t* GetCairoPattern() override { return pattern_; }

 private:
  Color color_;
  cairo_pattern_t* pattern_;
};
}  // namespace cru::platform::graphics::cairo
