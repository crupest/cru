#pragma once
#include "Base.h"

#include <cru/platform/graphics/Font.h>

#include <pango/pango.h>

namespace cru::platform::graphics::cairo {
class CRU_PLATFORM_GRAPHICS_CAIRO_API PangoFont : public CairoResource,
                                                  public virtual IFont {
 public:
  PangoFont(CairoGraphicsFactory* factory, std::string font_family, float font_size);

  ~PangoFont() override;

 public:
  std::string GetFontName() override;
  float GetFontSize() override;

  PangoFontDescription* GetPangoFontDescription() {
    return pango_font_description_;
  }

 private:
  std::string font_family_;
  float font_size_;
  PangoFontDescription* pango_font_description_;
};
}  // namespace cru::platform::graphics::cairo
