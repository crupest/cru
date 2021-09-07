#include "cru/osx/graphics/quartz/TextLayout.hpp"
#include "cru/osx/graphics/quartz/Convert.hpp"
#include "cru/osx/graphics/quartz/Resource.hpp"
#include "cru/platform/Check.hpp"

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

void OsxCTTextLayout::SetFont(std::shared_ptr<IFont> font) {
  font_ = CheckPlatform<OsxCTFont>(font, GetPlatformId());
  CFRelease(ct_framesetter_);
  CFRelease(ct_frame_);
  RecreateFrame();
}

void OsxCTTextLayout::SetText(String new_text) {
  text_ = std::move(new_text);
  CFRelease(cf_text_);
  cf_text_ = Convert(text_);
  CFRelease(ct_framesetter_);
  CFRelease(ct_frame_);
  RecreateFrame();
}

void OsxCTTextLayout::SetMaxWidth(float max_width) {
  max_width_ = max_width;
  CFRelease(ct_framesetter_);
  CFRelease(ct_frame_);
  RecreateFrame();
}

void OsxCTTextLayout::SetMaxHeight(float max_height) {
  max_height_ = max_height;
  CFRelease(ct_framesetter_);
  CFRelease(ct_frame_);
  RecreateFrame();
}

Rect OsxCTTextLayout::GetTextBounds(bool includingTrailingSpace) {
  auto lines = CTFrameGetLines(ct_frame_);

  const auto line_count = CFArrayGetCount(lines);

  for (int i = 0; i < line_count; i++) {
    auto line = CFArrayGetValueAtIndex(lines, i);

    // TODO: To be continued!
  }
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
