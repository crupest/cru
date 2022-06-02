#pragma once

#include "../Font.h"
#include "CairoResource.h"

namespace cru::platform::graphics::cairo {
class PangoFont : public CairoResource, public virtual IFont {
 public:
  PangoFont(CairoGraphicsFactory* factory, String font_family, float font_size);

  ~PangoFont() override;

 public:
  String GetFontName() override;
  float GetFontSize() override;

 private:
  String font_family_;
  float font_size_;
};
}  // namespace cru::platform::graphics::cairo
