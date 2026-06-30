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
  if (!font) {
    throw Exception("font can't be null.");
  }

  font_ = CheckPlatform<DWriteFont>(font, GetPlatformId());

  height_of_one_line_ = MeasureHeightOfOneLine();

  utf16_text_ = string::ToUtf16WString(text_);
  RecreateTextLayout();
}

DWriteTextLayout::~DWriteTextLayout() = default;

std::string DWriteTextLayout::GetText() { return text_; }

void DWriteTextLayout::SetText(std::string new_text) {
  text_ = std::move(new_text);
  utf16_text_ = string::ToUtf16WString(text_);
  RecreateTextLayout();
}

std::shared_ptr<IFont> DWriteTextLayout::GetFont() {
  return std::dynamic_pointer_cast<IFont>(font_);
}

void DWriteTextLayout::SetFont(std::shared_ptr<IFont> font) {
  font_ = CheckPlatform<DWriteFont>(font, GetPlatformId());
  RecreateTextLayout();
}

void DWriteTextLayout::SetMaxWidth(float max_width) {
  max_width_ = max_width;
  CheckHResult(text_layout_->SetMaxWidth(max_width_));
}

void DWriteTextLayout::SetMaxHeight(float max_height) {
  max_height_ = max_height;
  CheckHResult(text_layout_->SetMaxHeight(max_height_));
}

bool DWriteTextLayout::IsEditMode() { return edit_mode_; }

void DWriteTextLayout::SetEditMode(bool enable) { edit_mode_ = enable; }

Index DWriteTextLayout::GetLineIndexFromCharIndex(Index char_index) {
  if (char_index < 0 || char_index > text_.size()) {
    throw Exception("char_index is out of range.");
  }

  if (char_index == text_.size()) {
    return GetLineCount() - 1;
  }

  auto utf16_char_index = string::Utf16IndexCodePointToCodeUnit(
      utf16_text_.data(),
      string::Utf8IndexCodeUnitToCodePoint(text_, char_index));

  Index line_index = 0, line_end = 0;
  for (const auto& metrics : line_metrics_cache_) {
    line_end += static_cast<Index>(metrics.length);
    if (utf16_char_index < line_end) {
      return line_index;
    }
    line_index++;
  }

  std::unreachable();
}

float DWriteTextLayout::GetLineHeight(Index line_index) {
  if (line_index < 0 || line_index >= GetLineCount()) {
    throw Exception("line_index is out of range.");
  }
  return line_metrics_cache_[line_index].height;
}

Index DWriteTextLayout::GetLineCount() {
  return static_cast<Index>(line_metrics_cache_.size());
}

Rect DWriteTextLayout::GetTextBounds(bool includingTrailingSpace) {
  if (text_.empty()) {
    return GetEmptyTextBounds();
  }

  DWRITE_TEXT_METRICS metrics;
  CheckHResult(text_layout_->GetMetrics(&metrics));
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
      text_range_arg
          .TransformSides([&](Index index) {
            return string::Utf16IndexCodePointToCodeUnit(
                utf16_text_,
                string::Utf8IndexCodeUnitToCodePoint(text_, index));
          })
          .Normalize();

  DWRITE_TEXT_METRICS text_metrics;
  CheckHResult(text_layout_->GetMetrics(&text_metrics));
  const auto metrics_count =
      text_metrics.lineCount * text_metrics.maxBidiReorderingDepth;

  std::vector<DWRITE_HIT_TEST_METRICS> hit_test_metrics(metrics_count);
  UINT32 actual_count;
  CheckHResult(text_layout_->HitTestTextRange(
      static_cast<UINT32>(text_range.position),
      static_cast<UINT32>(text_range.count), 0, 0, hit_test_metrics.data(),
      metrics_count, &actual_count));
  hit_test_metrics.resize(actual_count);

  std::vector<Rect> result;
  result.reserve(actual_count);

  for (const auto& metrics : hit_test_metrics) {
    result.push_back(
        Rect{metrics.left, metrics.top, metrics.width, metrics.height});
  }

  return result;
}

Rect DWriteTextLayout::TextSinglePoint(Index position, bool trailing) {
  if (position < 0 || position > text_.size()) {
    throw Exception("position is out of range.");
  }

  if (text_.empty()) {
    return GetEmptyTextBounds();
  }

  position = string::Utf16IndexCodePointToCodeUnit(
      utf16_text_, string::Utf8IndexCodeUnitToCodePoint(text_, position));

  DWRITE_HIT_TEST_METRICS metrics;
  FLOAT left;
  FLOAT top;
  CheckHResult(text_layout_->HitTestTextPosition(static_cast<UINT32>(position),
                                                 static_cast<BOOL>(trailing),
                                                 &left, &top, &metrics));
  return Rect{left, top, 0, metrics.height};
}

TextHitTestResult DWriteTextLayout::HitTest(const Point& point) {
  BOOL trailing;
  BOOL inside;
  DWRITE_HIT_TEST_METRICS metrics;

  CheckHResult(text_layout_->HitTestPoint(point.x, point.y, &trailing, &inside,
                                          &metrics));

  TextHitTestResult result;
  result.position = string::Utf8IndexCodePointToCodeUnit(
      text_,
      string::Utf16IndexCodeUnitToCodePoint(utf16_text_, metrics.textPosition));
  result.trailing = trailing != 0;

  if (result.trailing) {
    Index position_with_trailing;
    string::Utf8NextCodePoint(text_.data(), text_.size(), result.position,
                              &position_with_trailing);
    result.position_with_trailing = position_with_trailing;
  } else {
    result.position_with_trailing = result.position;
  }

  return result;
}

void DWriteTextLayout::RecreateTextLayout() {
  CheckHResult(GetDirectFactory()->GetDWriteFactory()->CreateTextLayout(
      utf16_text_.c_str(), static_cast<UINT32>(utf16_text_.size()),
      font_->GetComInterface(), max_width_, max_height_, &text_layout_));

  UINT32 line_count = 0;
  text_layout_->GetLineMetrics(nullptr, 0, &line_count);
  line_metrics_cache_.resize(line_count);
  text_layout_->GetLineMetrics(line_metrics_cache_.data(), line_count,
                               &line_count);
}

float DWriteTextLayout::MeasureHeightOfOneLine() {
  // Measure the height of one line by creating a temporary text layout with
  // single line.
  Microsoft::WRL::ComPtr<IDWriteTextLayout> temp_text_layout;
  CheckHResult(GetDirectFactory()->GetDWriteFactory()->CreateTextLayout(
      L" ", 1, font_->GetComInterface(), std::numeric_limits<float>::max(),
      std::numeric_limits<float>::max(), &temp_text_layout));
  DWRITE_TEXT_METRICS metrics;
  CheckHResult(temp_text_layout->GetMetrics(&metrics));
  return metrics.height;
}

Rect DWriteTextLayout::GetEmptyTextBounds() {
  return edit_mode_ ? Rect{0, 0, 0, height_of_one_line_} : Rect{0, 0, 0, 0};
}
}  // namespace cru::platform::graphics::direct2d
