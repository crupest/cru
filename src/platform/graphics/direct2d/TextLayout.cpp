#include "cru/platform/graphics/direct2d/TextLayout.h"
#include "cru/base/StringUtil.h"
#include "cru/platform/graphics/direct2d/Factory.h"
#include "cru/platform/graphics/direct2d/Font.h"

#include <utility>

namespace cru::platform::graphics::direct2d {
DWriteTextLayout::DWriteTextLayout(DirectGraphicsFactory* factory,
                                   std::shared_ptr<IFont> font,
                                   std::string text)
    : DirectGraphicsResource(factory), text_(std::move(text)) {
  Expects(font);
  font_ = CheckPlatform<DWriteFont>(font, GetPlatformId());

  utf16_text_ = string::ToUtf16(text_);
  ThrowIfFailed(factory->GetDWriteFactory()->CreateTextLayout(
      utf16_text_.c_str(), static_cast<UINT32>(utf16_text_.size()),
      font_->GetComInterface(), max_width_, max_height_, &text_layout_));
}

DWriteTextLayout::~DWriteTextLayout() = default;

std::string DWriteTextLayout::GetText() { return text_; }

void DWriteTextLayout::SetText(std::string new_text) {
  text_ = std::move(new_text);
  utf16_text_ = string::ToUtf16(text_);
  ThrowIfFailed(GetDirectFactory()->GetDWriteFactory()->CreateTextLayout(
      utf16_text_.c_str(), static_cast<UINT32>(utf16_text_.size()),
      font_->GetComInterface(), max_width_, max_height_, &text_layout_));
}

std::shared_ptr<IFont> DWriteTextLayout::GetFont() {
  return std::dynamic_pointer_cast<IFont>(font_);
}

void DWriteTextLayout::SetFont(std::shared_ptr<IFont> font) {
  font_ = CheckPlatform<DWriteFont>(font, GetPlatformId());
  ThrowIfFailed(GetDirectFactory()->GetDWriteFactory()->CreateTextLayout(
      reinterpret_cast<const wchar_t*>(text_.c_str()),
      static_cast<UINT32>(text_.size()), font_->GetComInterface(), max_width_,
      max_height_, &text_layout_));
}

void DWriteTextLayout::SetMaxWidth(float max_width) {
  max_width_ = max_width;
  ThrowIfFailed(text_layout_->SetMaxWidth(max_width_));
}

void DWriteTextLayout::SetMaxHeight(float max_height) {
  max_height_ = max_height;
  ThrowIfFailed(text_layout_->SetMaxHeight(max_height_));
}

bool DWriteTextLayout::IsEditMode() { return edit_mode_; }

void DWriteTextLayout::SetEditMode(bool enable) {
  edit_mode_ = enable;
  // TODO: Implement this.
}

Index DWriteTextLayout::GetLineIndexFromCharIndex(Index char_index) {
  if (char_index < 0 || char_index >= text_.size()) {
    return -1;
  }

  auto line_index = 0;
  for (Index i = 0; i < char_index; ++i) {
    if (text_[i] == '\n') {
      line_index++;
    }
  }

  return line_index;
}

float DWriteTextLayout::GetLineHeight(Index line_index) {
  Index count = GetLineCount();
  std::vector<DWRITE_LINE_METRICS> line_metrics(count);

  UINT32 actual_line_count = 0;
  text_layout_->GetLineMetrics(line_metrics.data(), static_cast<UINT32>(count),
                               &actual_line_count);
  return line_metrics[line_index].height;
}

Index DWriteTextLayout::GetLineCount() {
  UINT32 line_count = 0;
  text_layout_->GetLineMetrics(nullptr, 0, &line_count);
  return line_count;
}

Rect DWriteTextLayout::GetTextBounds(bool includingTrailingSpace) {
  DWRITE_TEXT_METRICS metrics;
  ThrowIfFailed(text_layout_->GetMetrics(&metrics));
  return Rect{metrics.left, metrics.top,
              includingTrailingSpace ? metrics.widthIncludingTrailingWhitespace
                                     : metrics.width,
              metrics.height};
}

std::vector<Rect> DWriteTextLayout::TextRangeRect(
    const TextRange& text_range_arg) {
  if (text_range_arg.count == 0) {
    return {};
  }
  const auto text_range =
      Range::FromTwoSides(
          string::Utf16IndexCodePointToCodeUnit(
              reinterpret_cast<const char16_t*>(utf16_text_.data()),
              utf16_text_.size(),
              string::Utf8IndexCodeUnitToCodePoint(text_.data(), text_.size(),
                                                   text_range_arg.GetStart())),
          string::Utf16IndexCodePointToCodeUnit(
              reinterpret_cast<const char16_t*>(utf16_text_.data()),
              utf16_text_.size(),
              string::Utf8IndexCodeUnitToCodePoint(text_.data(), text_.size(),
                                                   text_range_arg.GetEnd())))
          .Normalize();

  DWRITE_TEXT_METRICS text_metrics;
  ThrowIfFailed(text_layout_->GetMetrics(&text_metrics));
  const auto metrics_count =
      text_metrics.lineCount * text_metrics.maxBidiReorderingDepth;

  std::vector<DWRITE_HIT_TEST_METRICS> hit_test_metrics(metrics_count);
  UINT32 actual_count;
  ThrowIfFailed(text_layout_->HitTestTextRange(
      static_cast<UINT32>(text_range.position),
      static_cast<UINT32>(text_range.count), 0, 0, hit_test_metrics.data(),
      metrics_count, &actual_count));

  hit_test_metrics.erase(hit_test_metrics.cbegin() + actual_count,
                         hit_test_metrics.cend());

  std::vector<Rect> result;
  result.reserve(actual_count);

  for (const auto& metrics : hit_test_metrics) {
    result.push_back(
        Rect{metrics.left, metrics.top, metrics.width, metrics.height});
  }

  return result;
}

Rect DWriteTextLayout::TextSinglePoint(Index position, bool trailing) {
  position = string::Utf16IndexCodePointToCodeUnit(
      reinterpret_cast<const char16_t*>(utf16_text_.data()), utf16_text_.size(),
      string::Utf8IndexCodeUnitToCodePoint(text_.data(), text_.size(),
                                           position));
  DWRITE_HIT_TEST_METRICS metrics;
  FLOAT left;
  FLOAT top;
  ThrowIfFailed(text_layout_->HitTestTextPosition(static_cast<UINT32>(position),
                                                  static_cast<BOOL>(trailing),
                                                  &left, &top, &metrics));
  return Rect{left, top, 0, GetFont()->GetFontSize()};
}

TextHitTestResult DWriteTextLayout::HitTest(const Point& point) {
  BOOL trailing;
  BOOL inside;
  DWRITE_HIT_TEST_METRICS metrics;

  ThrowIfFailed(text_layout_->HitTestPoint(point.x, point.y, &trailing, &inside,
                                           &metrics));

  TextHitTestResult result;
  result.position = string::Utf8IndexCodePointToCodeUnit(
      text_.data(), text_.size(),
      string::Utf16IndexCodeUnitToCodePoint(
          reinterpret_cast<const char16_t*>(utf16_text_.data()),
          utf16_text_.size(), metrics.textPosition));
  result.trailing = trailing != 0;
  return result;
}
}  // namespace cru::platform::graphics::direct2d
