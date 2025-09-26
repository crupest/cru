#include "cru/platform/graphics/cairo/PangoFont.h"

namespace cru::platform::graphics::cairo {
PangoFont::PangoFont(CairoGraphicsFactory* factory, String font_family,
                     float font_size)
    : CairoResource(factory),
      font_family_(std::move(font_family)),
      font_size_(font_size) {
  pango_font_description_ = pango_font_description_new();
  auto font_family_str = font_family_.ToUtf8();
  pango_font_description_set_family(pango_font_description_,
                                    font_family_str.c_str());
  pango_font_description_set_size(pango_font_description_, font_size * PANGO_SCALE);
}

PangoFont::~PangoFont() {
  pango_font_description_free(pango_font_description_);
}

String PangoFont::GetFontName() { return font_family_; }

float PangoFont::GetFontSize() { return font_size_; }
}  // namespace cru::platform::graphics::cairo
