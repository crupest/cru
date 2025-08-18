#include "cru/platform/graphics/quartz/Font.h"

#include "cru/base/Osx.h"
#include "cru/platform/graphics/quartz/Resource.h"

namespace cru::platform::graphics::quartz {
OsxCTFont::OsxCTFont(IGraphicsFactory* graphics_factory, const String& name,
                     float size)
    : OsxQuartzResource(graphics_factory), name_(name) {
  auto n = ToCFString(name);

  if (name.empty()) {
    ct_font_ =
        CTFontCreateUIFontForLanguage(kCTFontUIFontSystem, size, nullptr);
  } else {
    ct_font_ = CTFontCreateWithName(n.ref, size, nullptr);
  }
  Ensures(ct_font_);
}

OsxCTFont::~OsxCTFont() { CFRelease(ct_font_); }

String OsxCTFont::GetFontName() { return name_; }
float OsxCTFont::GetFontSize() { return CTFontGetSize(ct_font_); }
}  // namespace cru::platform::graphics::quartz
