// TODO: Trailing '\n' is not handled correctly until now. It had better be
// handled as an individual line, but is currently shown as nothing. This is due
// to Core Text's special handling of this. However, other '\n' are not affected
// by the problem.

#include "cru/platform/graphics/quartz/TextLayout.h"
#include "cru/base/Base.h"
#include "cru/base/StringUtil.h"
#include "cru/base/platform/osx/Base.h"

#include <algorithm>
#include <format>
#include <limits>
#include <utility>

namespace cru::platform::graphics::quartz {
OsxCTTextLayout::OsxCTTextLayout(IGraphicsFactory* graphics_factory,
                                 std::shared_ptr<OsxCTFont> font,
                                 const std::string& str)
    : OsxQuartzResource(graphics_factory),
      max_width_(std::numeric_limits<float>::max()),
      max_height_(std::numeric_limits<float>::max()),
      font_(std::move(font)),
      text_(str) {
  if (!font_) {
    throw Exception("font is null.");
  }

  height_of_one_line_ = MeasureHeightOfOneLine();

  DoSetText(std::move(text_));

  RecreateFrame();
}

OsxCTTextLayout::~OsxCTTextLayout() {
  ReleaseResource();
  CFRelease(cf_attributed_text_);
}

void OsxCTTextLayout::SetFont(std::shared_ptr<IFont> font) {
  font_ = CheckPlatform<OsxCTFont>(font, GetPlatformId());
  height_of_one_line_ = MeasureHeightOfOneLine();
  RecreateFrame();
}

void OsxCTTextLayout::DoSetText(std::string text) {
  text_ = std::move(text);

  cf_attributed_text_ = CFAttributedStringCreateMutable(nullptr, 0);
  CFAttributedStringReplaceString(cf_attributed_text_, CFRangeMake(0, 0),
                                  ToCFString(text_).Get());
  CFAttributedStringSetAttribute(
      cf_attributed_text_,
      CFRangeMake(0, CFAttributedStringGetLength(cf_attributed_text_)),
      kCTFontAttributeName, font_->GetCTFont());
}

void OsxCTTextLayout::SetText(std::string new_text) {
  if (new_text == text_) return;

  CFRelease(cf_attributed_text_);
  DoSetText(std::move(new_text));

  RecreateFrame();
}

void OsxCTTextLayout::SetMaxWidth(float max_width) {
  max_width_ = max_width;
  RecreateFrame();
}

void OsxCTTextLayout::SetMaxHeight(float max_height) {
  max_height_ = max_height;
  RecreateFrame();
}

bool OsxCTTextLayout::IsEditMode() { return edit_mode_; }

void OsxCTTextLayout::SetEditMode(bool enable) {
  if (edit_mode_ == enable) return;
  edit_mode_ = enable;
  RecreateFrame();
}

Index OsxCTTextLayout::GetLineIndexFromCharIndex(Index char_index) {
  if (char_index < 0 || char_index > text_.size()) {
    throw Exception("char_index is out of range.");
  }

  if (char_index == text_.size()) {
    return GetLineCount() - 1;
  }

  auto code_point_index =
      cru::string::Utf8IndexCodeUnitToCodePoint(text_, char_index);

  Index line_index = 0;
  for (const auto& line : lines_) {
    auto range = line.text_range;
    if (code_point_index >= range.location &&
        code_point_index < range.location + range.length) {
      return line_index;
    }
    line_index++;
  }

  std::unreachable();
}

Index OsxCTTextLayout::GetLineCount() { return lines_.size(); }

float OsxCTTextLayout::GetLineHeight(Index line_index) {
  if (line_index < 0 || line_index >= GetLineCount()) {
    throw Exception("line_index is out of range.");
  }
  return lines_[line_index].ascent + lines_[line_index].descent;
}

Rect OsxCTTextLayout::GetTextBounds(bool includingTrailingSpace) {
  auto result = DoGetTextBounds(includingTrailingSpace);
  return Rect(0, 0, result.size.width, result.size.height);
}

std::vector<Rect> OsxCTTextLayout::TextRangeRect(const TextRange& text_range) {
  auto results = DoTextRangeRect(text_range, false);
  std::vector<Rect> r;

  for (auto& rect : results) {
    r.push_back(transform_.TransformRect(Convert(rect)));
  }

  return r;
}

Rect OsxCTTextLayout::TextSinglePoint(Index position, bool trailing) {
  if (position < 0 || position > text_.size()) {
    throw Exception("position is out of range.");
  }

  auto result = DoTextSinglePoint(position, trailing);
  return transform_.TransformRect(Convert(result));
}

TextHitTestResult OsxCTTextLayout::HitTest(const Point& point) {
  if (text_.empty()) {
    TextHitTestResult result;
    result.position = 0;
    result.trailing = false;
    result.position_with_trailing = 0;
    result.inside_text = false;
    return result;
  }

  auto text_bounds = text_bounds_without_trailing_space_;
  CGPoint p = CGPointMake(
      point.x, text_bounds.origin.y + text_bounds.size.height - point.y);

  for (int i = 0; i < lines_.size(); i++) {
    const auto& line = lines_[i];
    auto line_top = line.origin.y + line.ascent;
    auto line_bottom = line.origin.y - line.descent;
    auto line_top_including_leading = line_top;
    if (i != 0) {
      line_top_including_leading += lines_[i - 1].leading;
    }
    if (!((i == 0 && p.y >= line_bottom) ||  // First line
          (i == lines_.size() - 1 &&
           p.y <= line_top_including_leading) ||  // Last line
          p.y >= line_bottom &&
              p.y <= line_top_including_leading)) {  // Middle lines
      continue;
    }

    auto x = std::clamp(p.x, line.origin.x, line.origin.x + line.width) -
             line.origin.x;
    auto y = std::clamp(p.y, line_bottom, line_top) - line.origin.y;

    auto index =
        CTLineGetStringIndexForPosition(line.ct_line, CGPointMake(x, y));

    auto line_bounds = CGRectMake(line.origin.x, line.origin.y - line.descent,
                                  line.width, line.ascent + line.descent);

    TextHitTestResult result;
    result.position = cru::string::Utf8IndexCodePointToCodeUnit(text_, index);
    result.trailing = false;
    result.position_with_trailing = result.position;
    result.inside_text = CGRectContainsPoint(line_bounds, p);
    return result;
  }

  std::unreachable();
}

void OsxCTTextLayout::ReleaseResource() {
  lines_.clear();
  ct_framesetter_ = {};
  ct_frame_ = {};
}

void OsxCTTextLayout::RecreateFrame() {
  ReleaseResource();

  ct_framesetter_.Reset(
      CTFramesetterCreateWithAttributedString(cf_attributed_text_));

  CFRange fit_range;

  suggest_height_ =
      CTFramesetterSuggestFrameSizeWithConstraints(
          ct_framesetter_.Get(),
          CFRangeMake(0, CFAttributedStringGetLength(cf_attributed_text_)),
          nullptr, CGSizeMake(max_width_, max_height_), &fit_range)
          .height;

  auto path = CFWrap(CGPathCreateMutable());
  CGPathAddRect(path.Get(), nullptr,
                CGRectMake(0, 0, max_width_, suggest_height_));

  ct_frame_.Reset(CTFramesetterCreateFrame(
      ct_framesetter_.Get(),
      CFRangeMake(0, CFAttributedStringGetLength(cf_attributed_text_)),
      path.Get(), nullptr));

  const auto lines = CTFrameGetLines(ct_frame_.Get());
  lines_.resize(CFArrayGetCount(lines));
  std::vector<CGPoint> line_origins(lines_.size());
  CTFrameGetLineOrigins(ct_frame_.Get(), CFRangeMake(0, 0),
                        line_origins.data());
  for (int i = 0; i < lines_.size(); i++) {
    auto& line = lines_[i];
    line.ct_line = static_cast<CTLineRef>(CFArrayGetValueAtIndex(lines, i));
    line.text_range = CTLineGetStringRange(line.ct_line);
    line.origin = line_origins[i];
    line.width = CTLineGetTypographicBounds(line.ct_line, &line.ascent,
                                            &line.descent, &line.leading);
  }

  auto bounds = DoGetTextBounds(false);
  text_bounds_without_trailing_space_ = bounds;
  text_bounds_with_trailing_space_ = DoGetTextBounds(true);

  auto right = bounds.origin.x + bounds.size.width;
  auto bottom = bounds.origin.y + bounds.size.height;

  transform_ = Matrix::Translation(-right / 2, -bottom / 2) *
               Matrix::Scale(1, -1) *
               Matrix::Translation(right / 2, bottom / 2);
}

CFWrapper<CTFrameRef> OsxCTTextLayout::CreateFrameWithColor(
    const Color& color) {
  auto path = CGPathCreateMutable();
  CGPathAddRect(path, nullptr, CGRectMake(0, 0, max_width_, suggest_height_));

  CGColorRef cg_color =
      CGColorCreateGenericRGB(color.GetFloatRed(), color.GetFloatGreen(),
                              color.GetFloatBlue(), color.GetFloatAlpha());
  CFAttributedStringSetAttribute(
      cf_attributed_text_,
      CFRangeMake(0, CFAttributedStringGetLength(cf_attributed_text_)),
      kCTForegroundColorAttributeName, cg_color);

  CFWrapper<CTFrameRef> frame(CTFramesetterCreateFrame(
      ct_framesetter_.Get(),
      CFRangeMake(0, CFAttributedStringGetLength(cf_attributed_text_)), path,
      nullptr));

  CGPathRelease(path);
  CGColorRelease(cg_color);

  return frame;
}

std::string OsxCTTextLayout::GetDebugString() {
  return std::format("OsxCTTextLayout(text: {}, size: ({}, {}))", text_,
                     max_width_, max_height_);
}

CGRect OsxCTTextLayout::DoGetTextBounds(bool includingTrailingSpace) {
  if (text_.empty()) return GetTextBoundsForEmptyString();

  auto rects = DoTextRangeRect(TextRange{0, static_cast<Index>(text_.size())},
                               includingTrailingSpace);

  float left = std::numeric_limits<float>::max();
  float bottom = std::numeric_limits<float>::max();
  float right = 0;
  float top = 0;

  for (auto& rect : rects) {
    if (rect.origin.x < left) left = rect.origin.x;
    if (rect.origin.y < bottom) bottom = rect.origin.y;
    if (rect.origin.x + rect.size.width > right)
      right = rect.origin.x + rect.size.width;
    if (rect.origin.y + rect.size.height > top)
      top = rect.origin.y + rect.size.height;
  }

  return CGRectMake(left, bottom, right - left, top - bottom);
}

std::vector<CGRect> OsxCTTextLayout::DoTextRangeRect(
    const TextRange& text_range, bool includingTrailingSpace) {
  if (text_.empty() || text_range.count == 0) return {};

  const auto r =
      text_range
          .TransformSides([&](Index index) {
            return cru::string::Utf8IndexCodeUnitToCodePoint(text_, index);
          })
          .Normalize();

  std::vector<CGRect> results;

  for (const auto& line : lines_) {
    auto line_range = r.CoerceInto(FromCFRange(line.text_range));

    if (line_range.count) {
      CGRect line_rect{line.origin.x, line.origin.y - line.descent, 0,
                       line.ascent + line.descent};
      auto start_offset = CTLineGetOffsetForStringIndex(
          line.ct_line, line_range.GetStart(), nullptr);
      auto end_offset = CTLineGetOffsetForStringIndex(
          line.ct_line, line_range.GetEnd(), nullptr);
      line_rect.origin.x += start_offset;
      line_rect.size.width = end_offset - start_offset;
      if (line_range.GetEnd() <= r.GetEnd() && includingTrailingSpace) {
        line_rect.size.width += line.trailing_whitespace_width;
      }
      results.push_back(line_rect);
    }
  }

  return results;
}

CGRect OsxCTTextLayout::DoTextSinglePoint(Index position, bool trailing) {
  if (position < 0 || position > text_.size()) {
    throw Exception("position is out of range.");
  }

  if (text_.empty()) return GetTextBoundsForEmptyString();

  bool is_end = position == text_.size();
  position = cru::string::Utf8IndexCodeUnitToCodePoint(text_, position);
  if (is_end) {
    position--;
    trailing = true;
  }

  for (const auto& line : lines_) {
    if (line.text_range.location <= position &&
        position < line.text_range.location + line.text_range.length) {
      double offset;
      if (trailing &&
          position == line.text_range.location + line.text_range.length - 1) {
        offset = line.width;
      } else {
        offset = CTLineGetOffsetForStringIndex(line.ct_line, position, nullptr);
      }
      return CGRectMake(line.origin.x + offset, line.origin.y - line.descent, 0,
                        line.ascent + line.descent);
    }
  }

  std::unreachable();
}

CGRect OsxCTTextLayout::GetTextBoundsForEmptyString() {
  // Keep empty text in edit mode at logical line height to avoid visual jumps
  // when first glyph appears.
  return edit_mode_ ? CGRectMake(0, 0, 0, height_of_one_line_)
                    : CGRectMake(0, 0, 0, 0);
}

double OsxCTTextLayout::MeasureHeightOfOneLine() {
  const auto ct_font = font_->GetCTFont();
  const auto ascent = CTFontGetAscent(ct_font);
  const auto descent = CTFontGetDescent(ct_font);
  const auto leading = std::max<CGFloat>(0, CTFontGetLeading(ct_font));
  return ascent + descent + leading;
}

}  // namespace cru::platform::graphics::quartz
