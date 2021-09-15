#include "cru/osx/graphics/quartz/Font.hpp"

#include "cru/osx/graphics/quartz/Convert.hpp"
#include "cru/osx/graphics/quartz/Resource.hpp"

namespace cru::platform::graphics::osx::quartz {
OsxCTFont::OsxCTFont(IGraphicsFactory* graphics_factory, const String& name,
                     float size)
    : OsxQuartzResource(graphics_factory) {
  CFStringRef n = Convert(name);

  ct_font_ = CTFontCreateWithName(n, size, nullptr);

  CFRelease(n);
}

OsxCTFont::~OsxCTFont() { CFRelease(ct_font_); }
}  // namespace cru::platform::graphics::osx::quartz
