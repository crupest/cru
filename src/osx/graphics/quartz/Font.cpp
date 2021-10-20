#include "cru/osx/graphics/quartz/Font.hpp"

#include "cru/osx/Convert.hpp"
#include "cru/osx/graphics/quartz/Convert.hpp"
#include "cru/osx/graphics/quartz/Resource.hpp"

namespace cru::platform::graphics::osx::quartz {
using cru::platform::osx::Convert;

OsxCTFont::OsxCTFont(IGraphicsFactory* graphics_factory, const String& name,
                     float size)
    : OsxQuartzResource(graphics_factory) {
  CFStringRef n = Convert(name);

  ct_font_ = CTFontCreateWithName(n, size, nullptr);
  Ensures(ct_font_);

  CFRelease(n);
}

OsxCTFont::~OsxCTFont() { CFRelease(ct_font_); }

float OsxCTFont::GetFontSize() { return CTFontGetSize(ct_font_); }
}  // namespace cru::platform::graphics::osx::quartz
