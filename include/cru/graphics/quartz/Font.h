#pragma once
#include "Resource.h"
#include <cru/Base.h>
#include "cru/graphics/Font.h"

#include <CoreText/CoreText.h>

namespace cru::graphics::quartz {
class OsxCTFont : public OsxQuartzResource, public virtual IFont {
 public:
  OsxCTFont(IGraphicsFactory* graphics_factory, const String& name, float size);


  ~OsxCTFont() override;

  CTFontRef GetCTFont() const { return ct_font_; }

  String GetFontName() override;
  float GetFontSize() override;

 private:
  String name_;
  CTFontRef ct_font_;
};
}  // namespace cru::graphics::quartz
