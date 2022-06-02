#include "cru/platform/graphics/cairo/PangoFont.h"

namespace cru::platform::graphics::cairo {
PangoFont::PangoFont(CairoGraphicsFactory* factory, String font_family,
                     float font_size)
    : CairoResource(factory),
      font_family_(std::move(font_family)),
      font_size_(font_size) {}

PangoFont::~PangoFont() {}

String PangoFont::GetFontName() { return font_family_; }

float PangoFont::GetFontSize() { return font_size_; }
}  // namespace cru::platform::graphics::cairo
