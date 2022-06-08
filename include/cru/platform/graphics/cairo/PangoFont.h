#pragma once

#include "../Font.h"
#include "CairoResource.h"

#include <pango/pango.h>

namespace cru::platform::graphics::cairo {
class CRU_PLATFORM_GRAPHICS_CAIRO_API PangoFont : public CairoResource,
                                                  public virtual IFont {
 public:
  PangoFont(CairoGraphicsFactory* factory, String font_family, float font_size);

  ~PangoFont() override;

 public:
  String GetFontName() override;
  float GetFontSize() override;

  PangoFontDescription* GetPangoFontDescription() {
    return pango_font_description_;
  }

 private:
  String font_family_;
  float font_size_;
  PangoFontDescription* pango_font_description_;
};
}  // namespace cru::platform::graphics::cairo
