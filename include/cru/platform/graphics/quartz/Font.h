#pragma once
#include "Resource.h"
#include "cru/base/Base.h"
#include "cru/platform/graphics/Font.h"

#include <CoreText/CoreText.h>

namespace cru::platform::graphics::quartz {
class OsxCTFont : public OsxQuartzResource, public virtual IFont {
 public:
  OsxCTFont(IGraphicsFactory* graphics_factory, const String& name, float size);

  CRU_DELETE_COPY(OsxCTFont)
  CRU_DELETE_MOVE(OsxCTFont)

  ~OsxCTFont() override;

  CTFontRef GetCTFont() const { return ct_font_; }

  String GetFontName() override;
  float GetFontSize() override;

 private:
  String name_;
  CTFontRef ct_font_;
};
}  // namespace cru::platform::graphics::quartz
