#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"
#include "cru/platform/graphics/cairo/CairoBrush.h"
#include "cru/platform/graphics/cairo/CairoGeometry.h"
#include "cru/platform/graphics/cairo/CairoImageFactory.h"
#include "cru/platform/graphics/cairo/CairoResource.h"
#include "cru/platform/graphics/cairo/PangoFont.h"
#include "cru/platform/graphics/cairo/PangoTextLayout.h"

namespace cru::platform::graphics::cairo {
CairoGraphicsFactory::CairoGraphicsFactory() : CairoResource(this) {
  default_cairo_surface_ =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 100, 100);
  default_cairo_ = cairo_create(default_cairo_surface_);
  default_pango_context_ = pango_context_new();

  image_factory_ = std::make_unique<CairoImageFactory>(this);
}

CairoGraphicsFactory::~CairoGraphicsFactory() {
  g_object_unref(default_pango_context_);
  cairo_destroy(default_cairo_);
  cairo_surface_destroy(default_cairo_surface_);
}

std::unique_ptr<ISolidColorBrush>
CairoGraphicsFactory::CreateSolidColorBrush() {
  return std::make_unique<CairoSolidColorBrush>(this);
}

std::unique_ptr<IGeometryBuilder>
CairoGraphicsFactory::CreateGeometryBuilder() {
  return std::make_unique<CairoGeometryBuilder>(this);
}

std::unique_ptr<IFont> CairoGraphicsFactory::CreateFont(String font_family,
                                                        float font_size) {
  return std::make_unique<PangoFont>(this, std::move(font_family), font_size);
}

std::unique_ptr<ITextLayout> CairoGraphicsFactory::CreateTextLayout(
    std::shared_ptr<IFont> font, String text) {
  auto text_layout = std::make_unique<PangoTextLayout>(this, std::move(font));
  text_layout->SetText(std::move(text));
  return text_layout;
}

IImageFactory* CairoGraphicsFactory::GetImageFactory() {
  return image_factory_.get();
}
}  // namespace cru::platform::graphics::cairo
