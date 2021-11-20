#include "cru/osx/graphics/quartz/TextLayout.hpp"
#include "cru/common/StringUtil.hpp"
#include "cru/osx/Convert.hpp"
#include "cru/osx/graphics/quartz/Convert.hpp"
#include "cru/osx/graphics/quartz/Resource.hpp"
#include "cru/platform/Check.hpp"
#include "cru/platform/graphics/Base.hpp"

#include <algorithm>
#include <limits>

namespace cru::platform::graphics::osx::quartz {
using cru::platform::osx::Convert;

OsxCTTextLayout::OsxCTTextLayout(IGraphicsFactory* graphics_factory,
                                 std::shared_ptr<OsxCTFont> font,
                                 const String& str)
    : OsxQuartzResource(graphics_factory),
      max_width_(std::numeric_limits<float>::max()),
      max_height_(std::numeric_limits<float>::max()),
      font_(std::move(font)),
      text_(str) {
  Expects(font_);

  CFStringRef s = Convert(text_);
  cf_attributed_text_ = CFAttributedStringCreateMutable(nullptr, 0);
  CFAttributedStringReplaceString(cf_attributed_text_, CFRangeMake(0, 0), s);
  Ensures(cf_attributed_text_);
  CFAttributedStringSetAttribute(
      cf_attributed_text_,
      CFRangeMake(0, CFAttributedStringGetLength(cf_attributed_text_)),
      kCTFontAttributeName, font_->GetCTFont());
  CFRelease(s);

  RecreateFrame();
}

OsxCTTextLayout::~OsxCTTextLayout() {
  ReleaseResource();
  CFRelease(cf_attributed_text_);
}

void OsxCTTextLayout::SetFont(std::shared_ptr<IFont> font) {
  font_ = CheckPlatform<OsxCTFont>(font, GetPlatformId());
  RecreateFrame();
}

void OsxCTTextLayout::SetText(String new_text) {
  if (new_text == text_) return;

  text_ = std::move(new_text);

  if (text_.empty()) {
    head_empty_line_count_ = 0;
    tail_empty_line_count_ = 1;

    actual_text_ = {};
  } else {
    head_empty_line_count_ = 0;
    tail_empty_line_count_ = 0;

    for (auto i = text_.cbegin(); i != text_.cend(); ++i) {
      if (*i == u'\n') {
        head_empty_line_count_++;
      } else {
        break;
      }
    }

    for (auto i = text_.crbegin(); i != text_.crend(); ++i) {
      if (*i == u'\n') {
        tail_empty_line_count_++;
      } else {
        break;
      }
    }

    if (text_.size() == tail_empty_line_count_) {
      head_empty_line_count_ = 1;
      actual_text_ = {};
    } else {
      actual_text_ = String(text_.cbegin() + head_empty_line_count_,
                            text_.cend() - tail_empty_line_count_);
    }
  }

  CFStringRef s = Convert(actual_text_);
  cf_attributed_text_ = CFAttributedStringCreateMutable(nullptr, 0);
  CFAttributedStringReplaceString(cf_attributed_text_, CFRangeMake(0, 0), s);
  Ensures(cf_attributed_text_);
  CFAttributedStringSetAttribute(
      cf_attributed_text_,
      CFRangeMake(0, CFAttributedStringGetLength(cf_attributed_text_)),
      kCTFontAttributeName, font_->GetCTFont());
  CFRelease(s);

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
  edit_mode_ = enable;
  RecreateFrame();
}

Rect OsxCTTextLayout::GetTextBounds(bool includingTrailingSpace) {
  if (text_.empty() && edit_mode_) return Rect(0, 0, 0, font_->GetFontSize());

  auto result = DoGetTextBoundsIncludingEmptyLines(includingTrailingSpace);
  return Rect(0, 0, result.size.width, result.size.height);
}

std::vector<Rect> OsxCTTextLayout::TextRangeRect(const TextRange& text_range) {
  if (text_.empty()) return {};

  auto tr = text_range;
  tr.position += head_empty_line_count_;
  std::vector<CGRect> results = DoTextRangeRect(tr);
  std::vector<Rect> r;

  for (auto& rect : results) {
    r.push_back(transform_.TransformRect(Convert(rect)));
  }

  return r;
}

Rect OsxCTTextLayout::TextSinglePoint(Index position, bool trailing) {
  Expects(position >= 0 && position <= text_.size());

  if (text_.empty()) return {0, 0, 0, font_->GetFontSize()};

  if (position < head_empty_line_count_) {
    return {0, position * font_->GetFontSize(), 0, font_->GetFontSize()};
  } else if (position > text_.size() - tail_empty_line_count_) {
    return {
        0,
        static_cast<float>(text_bounds_without_trailing_space_.size.height) +
            (head_empty_line_count_ + position -
             (text_.size() - tail_empty_line_count_) - 1) *
                font_->GetFontSize(),
        0, font_->GetFontSize()};
  } else {
    auto result =
        DoTextSinglePoint(position - head_empty_line_count_, trailing);
    return transform_.TransformRect(Convert(result));
  }
}

TextHitTestResult OsxCTTextLayout::HitTest(const Point& point) {
  if (point.y < head_empty_line_count_ * font_->GetFontSize()) {
    if (point.y < 0) {
      return {0, false, false};
    } else {
      for (int i = 0; i < head_empty_line_count_; ++i) {
        if (point.y < i * font_->GetFontSize()) {
          return {i, false, false};
        }
      }
    }
  }

  auto text_bounds = text_bounds_without_trailing_space_;

  auto text_height = static_cast<float>(text_bounds.size.height);
  auto th = text_height + head_empty_line_count_ * font_->GetFontSize();
  if (point.y >= th) {
    for (int i = 0; i < tail_empty_line_count_; ++i) {
      if (point.y < th + i * font_->GetFontSize()) {
        return {text_.size() - i, false, false};
      }
    }
  }

  auto p = point;
  p.y -= head_empty_line_count_ * font_->GetFontSize();
  p.x += text_bounds.origin.x;
  p.y += text_bounds.origin.y;
  p.y = text_height - p.y;

  for (int i = 0; i < line_count_; i++) {
    auto line = lines_[i];
    const auto& line_origin = line_origins_[i];

    auto range = actual_text_.RangeFromCodePointToCodeUnit(
        Convert(CTLineGetStringRange(line)));

    auto bounds = CTLineGetImageBounds(line, nullptr);
    bounds.origin.x += line_origin.x;
    bounds.origin.y += line_origin.y;

    bool force_inside = false;
    if (i == 0 && p.y >= bounds.origin.y + bounds.size.height) {
      force_inside = true;
    }

    if (i == line_count_ - 1 && p.y < bounds.origin.y) {
      force_inside = true;
    }

    if (p.y >= bounds.origin.y || force_inside) {
      auto pp = p;
      pp.y = bounds.origin.y;

      if (pp.x < bounds.origin.x) {
        return TextHitTestResult{range.position + head_empty_line_count_, false,
                                 false};
      } else if (pp.x > bounds.origin.x + bounds.size.width) {
        auto po = text_.IndexFromCodePointToCodeUnit(range.GetEnd());
        if (po != text_.size()) {
          Utf16PreviousCodePoint(text_, po, &po);
        }
        return TextHitTestResult{po + head_empty_line_count_, false, false};
      } else {
        int position = CTLineGetStringIndexForPosition(
            line, CGPointMake(pp.x - line_origin.x, pp.y - line_origin.y));
        return TextHitTestResult{text_.IndexFromCodePointToCodeUnit(position) +
                                     head_empty_line_count_,
                                 false, true};
      }
    }
  }

  return TextHitTestResult{0, false, false};
}

void OsxCTTextLayout::ReleaseResource() {
  line_count_ = 0;
  line_origins_.clear();
  lines_.clear();
  if (ct_framesetter_) CFRelease(ct_framesetter_);
  if (ct_frame_) CFRelease(ct_frame_);
}

void OsxCTTextLayout::RecreateFrame() {
  ReleaseResource();

  ct_framesetter_ =
      CTFramesetterCreateWithAttributedString(cf_attributed_text_);
  Ensures(ct_framesetter_);

  CFRange fit_range;

  suggest_height_ =
      CTFramesetterSuggestFrameSizeWithConstraints(
          ct_framesetter_,
          CFRangeMake(0, CFAttributedStringGetLength(cf_attributed_text_)),
          nullptr, CGSizeMake(max_width_, max_height_), &fit_range)
          .height;

  auto path = CGPathCreateMutable();
  Ensures(path);
  CGPathAddRect(path, nullptr, CGRectMake(0, 0, max_width_, suggest_height_));

  ct_frame_ = CTFramesetterCreateFrame(
      ct_framesetter_,
      CFRangeMake(0, CFAttributedStringGetLength(cf_attributed_text_)), path,
      nullptr);
  Ensures(ct_frame_);

  CGPathRelease(path);

  const auto lines = CTFrameGetLines(ct_frame_);
  line_count_ = CFArrayGetCount(lines);
  lines_.resize(line_count_);
  line_origins_.resize(line_count_);
  CTFrameGetLineOrigins(ct_frame_, CFRangeMake(0, 0), line_origins_.data());
  for (int i = 0; i < line_count_; i++) {
    lines_[i] = static_cast<CTLineRef>(CFArrayGetValueAtIndex(lines, i));
  }

  auto bounds = DoGetTextBounds(false);
  text_bounds_without_trailing_space_ = bounds;
  text_bounds_with_trailing_space_ = DoGetTextBounds(true);

  transform_ =
      Matrix::Translation(-bounds.origin.x, -bounds.origin.y) *
      Matrix::Translation(-bounds.size.width / 2, -bounds.size.height / 2) *
      Matrix::Scale(1, -1) *
      Matrix::Translation(bounds.size.width / 2, bounds.size.height / 2) *
      Matrix::Translation(0, head_empty_line_count_ * font_->GetFontSize());
}

CTFrameRef OsxCTTextLayout::CreateFrameWithColor(const Color& color) {
  auto path = CGPathCreateMutable();
  CGPathAddRect(path, nullptr, CGRectMake(0, 0, max_width_, suggest_height_));

  CGColorRef cg_color =
      CGColorCreateGenericRGB(color.GetFloatRed(), color.GetFloatGreen(),
                              color.GetFloatBlue(), color.GetFloatAlpha());
  CFAttributedStringSetAttribute(
      cf_attributed_text_,
      CFRangeMake(0, CFAttributedStringGetLength(cf_attributed_text_)),
      kCTForegroundColorAttributeName, cg_color);

  auto frame = CTFramesetterCreateFrame(
      ct_framesetter_,
      CFRangeMake(0, CFAttributedStringGetLength(cf_attributed_text_)), path,
      nullptr);
  Ensures(frame);

  CGPathRelease(path);

  return frame;
}

String OsxCTTextLayout::GetDebugString() {
  return Format(u"OsxCTTextLayout(text: {}, size: ({}, {}))", text_, max_width_,
                max_height_);
}

CGRect OsxCTTextLayout::DoGetTextBounds(bool includingTrailingSpace) {
  if (actual_text_.empty()) return CGRect{};

  float left = std::numeric_limits<float>::max();
  float bottom = std::numeric_limits<float>::max();
  float right = 0;
  float top = 0;

  for (int i = 0; i < line_count_; i++) {
    auto line = lines_[i];
    const auto& line_origin = line_origins_[i];

    CGRect line_rect = CTLineGetImageBounds(line, nullptr);
    if (includingTrailingSpace) {
      float trailingWidth = CTLineGetTrailingWhitespaceWidth(line);
      line_rect.size.width += trailingWidth;
    }

    line_rect.origin.x += line_origin.x;
    line_rect.origin.y += line_origin.y;

    left = std::min<float>(line_rect.origin.x, left);
    bottom = std::min<float>(line_rect.origin.y, bottom);
    right = std::max<float>(line_rect.origin.x + line_rect.size.width, right);
    top = std::max<float>(line_rect.origin.y + line_rect.size.height, top);
  }

  return CGRectMake(left, bottom, right - left, top - bottom);
}

CGRect OsxCTTextLayout::DoGetTextBoundsIncludingEmptyLines(
    bool includingTrailingSpace) {
  auto result = includingTrailingSpace ? text_bounds_with_trailing_space_
                                       : text_bounds_without_trailing_space_;

  result.size.height += head_empty_line_count_ * font_->GetFontSize();
  result.size.height += tail_empty_line_count_ * font_->GetFontSize();

  return result;
}

std::vector<CGRect> OsxCTTextLayout::DoTextRangeRect(
    const TextRange& text_range) {
  const auto r = text_.RangeFromCodeUnitToCodePoint(text_range).Normalize();

  std::vector<CGRect> results;

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
      results.push_back(line_rect);
    }
  }

  return results;
}

CGRect OsxCTTextLayout::DoTextSinglePoint(Index position, bool trailing) {
  Expects(position >= 0 && position <= actual_text_.size());

  if (actual_text_.empty()) return CGRectMake(0, 0, 0, font_->GetFontSize());

  position = actual_text_.IndexFromCodeUnitToCodePoint(position);
  for (int i = 0; i < line_count_; i++) {
    auto line = lines_[i];
    const auto& line_origin = line_origins_[i];

    auto rect = CTLineGetImageBounds(line, nullptr);
    rect.origin.x += line_origin.x;
    rect.origin.y += line_origin.y;

    Range range = Convert(CTLineGetStringRange(line));
    if (range.GetStart() <= position && position < range.GetEnd()) {
      auto offset = CTLineGetOffsetForStringIndex(line, position, nullptr);
      return CGRectMake(rect.origin.x + offset, rect.origin.y, 0,
                        rect.size.height);
    }
  }

  auto rect = CTLineGetImageBounds(lines_.back(), nullptr);
  rect.origin.x += line_origins_.back().x;
  rect.origin.y += line_origins_.back().y;
  return CGRectMake(rect.origin.x + rect.size.width, rect.origin.y, 0,
                    rect.size.height);
}
}  // namespace cru::platform::graphics::osx::quartz
