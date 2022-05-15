#include "cru/platform/graphics/quartz/Font.h"

#include "cru/platform/osx/Convert.h"
#include "cru/platform/graphics/quartz/Convert.h"
#include "cru/platform/graphics/quartz/Resource.h"

namespace cru::platform::graphics::quartz {
using cru::platform::osx::Convert;

OsxCTFont::OsxCTFont(IGraphicsFactory* graphics_factory, const String& name,
                     float size)
    : OsxQuartzResource(graphics_factory), name_(name) {
  CFStringRef n = Convert(name);

  if (name.empty()) {
    ct_font_ =
        CTFontCreateUIFontForLanguage(kCTFontUIFontSystem, size, nullptr);
  } else {
    ct_font_ = CTFontCreateWithName(n, size, nullptr);
  }
  Ensures(ct_font_);

  CFRelease(n);
}

OsxCTFont::~OsxCTFont() { CFRelease(ct_font_); }

String OsxCTFont::GetFontName() { return name_; }
float OsxCTFont::GetFontSize() { return CTFontGetSize(ct_font_); }
}  // namespace cru::platform::graphics::quartz
