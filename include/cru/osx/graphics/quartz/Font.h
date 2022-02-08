#pragma once
#include "Resource.h"
#include "cru/common/Base.h"
#include "cru/platform/graphics/Font.h"

#include <CoreText/CoreText.h>

namespace cru::platform::graphics::osx::quartz {
class OsxCTFont : public OsxQuartzResource, public virtual IFont {
 public:
  OsxCTFont(IGraphicsFactory* graphics_factory, const String& name, float size);

  CRU_DELETE_COPY(OsxCTFont)
  CRU_DELETE_MOVE(OsxCTFont)

  ~OsxCTFont() override;

  CTFontRef GetCTFont() const { return ct_font_; }

  float GetFontSize() override;

 private:
  CTFontRef ct_font_;
};
}  // namespace cru::platform::graphics::osx::quartz
