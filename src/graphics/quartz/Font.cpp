#include "cru/graphics/quartz/Font.h"
#include <cru/Osx.h>
#include "cru/graphics/quartz/Resource.h"

namespace cru::graphics::quartz {
OsxCTFont::OsxCTFont(IGraphicsFactory* graphics_factory, const String& name,
                     float size)
    : OsxQuartzResource(graphics_factory), name_(name) {
  if (name.empty()) {
    ct_font_ =
        CTFontCreateUIFontForLanguage(kCTFontUIFontSystem, size, nullptr);
  } else {
    ct_font_ = CTFontCreateWithName(::cru::osx::ToCF(name).ref, size, nullptr);
  }
  Ensures(ct_font_);
}

OsxCTFont::~OsxCTFont() { CFRelease(ct_font_); }

String OsxCTFont::GetFontName() { return name_; }
float OsxCTFont::GetFontSize() { return CTFontGetSize(ct_font_); }
}  // namespace cru::graphics::quartz
