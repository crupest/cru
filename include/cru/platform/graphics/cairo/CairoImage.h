#pragma once
#include "../Image.h"
#include "CairoResource.h"

namespace cru::platform::graphics::cairo {
class CRU_PLATFORM_GRAPHICS_CAIRO_API CairoImage : public CairoResource,
                                                   public virtual IImage {
 public:
  CairoImage(CairoGraphicsFactory* factory, int width, int height);
  CairoImage(CairoGraphicsFactory* factory,
             cairo_surface_t* cairo_image_surface, bool auto_release);

  ~CairoImage() override;

 public:
  float GetWidth() override;
  float GetHeight() override;

  std::unique_ptr<IImage> CreateWithRect(const Rect& rect) override;

  std::unique_ptr<IPainter> CreatePainter() override;

  cairo_surface_t* GetCairoSurface() { return cairo_surface_; }

 private:
  cairo_surface_t* cairo_surface_;
  bool auto_release_;
};
}  // namespace cru::platform::graphics::cairo
