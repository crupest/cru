#include "cru/osx/graphics/quartz/TextLayout.hpp"
#include "cru/osx/graphics/quartz/Convert.hpp"
#include "cru/osx/graphics/quartz/Resource.hpp"

namespace cru::platform::graphics::osx::quartz {
OsxCTTextLayout::OsxCTTextLayout(IGraphFactory* graphics_factory,
                                 std::shared_ptr<OsxCTFont> font,
                                 const String& str)
    : OsxQuartzResource(graphics_factory),
      max_width_(0.f),
      max_height_(0.f),
      font_(std::move(font)),
      text_(str) {
  cf_text_ = Convert(str);

  RecreateFrame();
}

void OsxCTTextLayout::SetText(String new_text) {
  text_ = std::move(new_text);
  CFRelease(cf_text_);
  cf_text_ = Convert(text_);
  RecreateFrame();
}

void OsxCTTextLayout::RecreateFrame() {
  auto attributed_string = CFAttributedStringCreateMutable(nullptr, 0);
  CFAttributedStringReplaceString(attributed_string, CFRangeMake(0, 0),
                                  cf_text_);
  CFAttributedStringSetAttribute(attributed_string,
                                 CFRangeMake(0, CFStringGetLength(cf_text_)),
                                 kCTFontAttributeName, font_->GetCTFont());
  ct_framesetter_ = CTFramesetterCreateWithAttributedString(attributed_string);

  auto path = CGPathCreateMutable();
  CGPathAddRect(path, nullptr, CGRectMake(0, 0, max_width_, max_height_));

  ct_frame_ = CTFramesetterCreateFrame(
      ct_framesetter_, CFRangeMake(0, CFStringGetLength(cf_text_)), path,
      nullptr);

  CFRelease(attributed_string);
  CGPathRelease(path);
}
}  // namespace cru::platform::graphics::osx::quartz
