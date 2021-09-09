#include "cru/osx/graphics/quartz/TextLayout.hpp"
#include "cru/osx/graphics/quartz/Convert.hpp"
#include "cru/osx/graphics/quartz/Resource.hpp"
#include "cru/platform/Check.hpp"

#include <algorithm>
#include <limits>

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
  float left = std::numeric_limits<float>::max();
  float top = std::numeric_limits<float>::max();
  float right = 0;
  float bottom = 0;

  for (int i = 0; i < line_count_; i++) {
    auto line = static_cast<CTLineRef>(lines_[i]);
    const auto& line_origin = line_origins_[i];

    CGRect line_rect = CTLineGetImageBounds(line, nullptr);
    if (includingTrailingSpace) {
      float trailingWidth = CTLineGetTrailingWhitespaceWidth(line);
      line_rect.size.width += trailingWidth;
    }

    line_rect.origin.x += line_origin.x;
    line_rect.origin.y += line_origin.y;

    left = std::min<float>(line_rect.origin.x, left);
    top = std::min<float>(line_rect.origin.y, top);
    right = std::max<float>(line_rect.origin.x + line_rect.size.width, right);
    bottom =
        std::max<float>(line_rect.origin.y + line_rect.size.height, bottom);
  }

  return Rect::FromVertices(left, top, right, bottom);
}

std::vector<Rect> OsxCTTextLayout::TextRangeRect(const TextRange& text_range) {
  const auto r = text_.RangeFromCodeUnitToCodePoint(text_range).Normalize();

  std::vector<Rect> results;

  for (int i = 0; i < line_count_; i++) {
    auto line = lines_[i];
    const auto& line_origin = line_origins_[i];

    Range range = Convert(CTLineGetStringRange(line));
    range = range.CoerceInto(r.GetStart(), r.GetEnd());

    if (range.count) {
      auto line_rect = CTLineGetImageBounds(line, nullptr);
      line_rect.origin.x += line_origin.x;
      line_rect.origin.y += line_origin.y;
      float start_offset =
          CTLineGetOffsetForStringIndex(line, range.GetStart(), nullptr);
      float end_offset =
          CTLineGetOffsetForStringIndex(line, range.GetEnd(), nullptr);
      line_rect.origin.x += start_offset;
      line_rect.size.width = end_offset - start_offset;
      results.push_back(Convert(line_rect));
    }
  }

  return results;
}

Point OsxCTTextLayout::TextSinglePoint(Index position, bool trailing) {
  position = text_.IndexFromCodeUnitToCodePoint(position);
  for (int i = 0; i < line_count_; i++) {
    auto line = lines_[i];
    const auto& line_origin = line_origins_[i];

    Range range = Convert(CTLineGetStringRange(line));
    if (range.GetStart() <= position && position < range.GetEnd()) {
      auto offset = CTLineGetOffsetForStringIndex(line, position, nullptr);
      return Point(line_origin.x + offset, line_origin.y);
    }
  }

  return Convert(CTLineGetImageBounds(lines_.back(), nullptr)).GetRightTop();
}

TextHitTestResult OsxCTTextLayout::HitTest(const Point& point) {
  for (int i = 0; i < line_count_; i++) {
    auto line = lines_[i];
    const auto& line_origin = line_origins_[i];

    auto bounds = Convert(CTLineGetImageBounds(line, nullptr));
    bounds.left += line_origin.x;
    bounds.top += line_origin.y;
    if (bounds.IsPointInside(point)) {
      int position = CTLineGetStringIndexForPosition(
          line, CGPointMake(point.x - line_origin.x, point.y - line_origin.y));
      return TextHitTestResult{position, false, true};
    }
  }

  return TextHitTestResult{0, false, false};
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

  const auto lines = CTFrameGetLines(ct_frame_);
  line_count_ = CFArrayGetCount(lines);
  lines_.resize(line_count_);
  line_origins_.resize(line_count_);
  CTFrameGetLineOrigins(ct_frame_, CFRangeMake(0, 0), line_origins_.data());
  for (int i = 0; i < line_count_; i++) {
    lines_[i] = static_cast<CTLineRef>(CFArrayGetValueAtIndex(lines, i));
  }
}

}  // namespace cru::platform::graphics::osx::quartz
