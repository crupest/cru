#pragma once
#include "Resource.h"
#include "cru/base/Base.h"
#include "cru/platform/graphics/Font.h"

#include <CoreText/CoreText.h>

namespace cru::platform::graphics::quartz {
class OsxCTFont : public OsxQuartzResource, public virtual IFont {
 public:
  OsxCTFont(IGraphicsFactory* graphics_factory, const std::string& name, float size);

  CRU_DELETE_COPY(OsxCTFont)
  CRU_DELETE_MOVE(OsxCTFont)

  ~OsxCTFont() override;

  CTFontRef GetCTFont() const { return ct_font_; }

  std::string GetFontName() override;
  float GetFontSize() override;

 private:
  std::string name_;
  CTFontRef ct_font_;
};
}  // namespace cru::platform::graphics::quartz
