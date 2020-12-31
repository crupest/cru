#include "cru/win/graphics/direct/TextLayout.hpp"

#include "cru/common/Logger.hpp"
#include "cru/platform/Check.hpp"
#include "cru/win/graphics/direct/Exception.hpp"
#include "cru/win/graphics/direct/Factory.hpp"
#include "cru/win/graphics/direct/Font.hpp"

#include <utility>

namespace cru::platform::graphics::win::direct {
DWriteTextLayout::DWriteTextLayout(DirectGraphFactory* factory,
                                   std::shared_ptr<IFont> font,
                                   std::u16string text)
    : DirectGraphResource(factory), text_(std::move(text)) {
  Expects(font);
  font_ = CheckPlatform<DWriteFont>(font, GetPlatformId());

  ThrowIfFailed(factory->GetDWriteFactory()->CreateTextLayout(
      reinterpret_cast<const wchar_t*>(text_.c_str()),
      static_cast<UINT32>(text_.size()), font_->GetComInterface(), max_width_,
      max_height_, &text_layout_));
}

DWriteTextLayout::~DWriteTextLayout() = default;

std::u16string DWriteTextLayout::GetText() { return text_; }

std::u16string_view DWriteTextLayout::GetTextView() { return text_; }

void DWriteTextLayout::SetText(std::u16string new_text) {
  text_.swap(new_text);
  ThrowIfFailed(GetDirectFactory()->GetDWriteFactory()->CreateTextLayout(
      reinterpret_cast<const wchar_t*>(text_.c_str()),
      static_cast<UINT32>(text_.size()), font_->GetComInterface(), max_width_,
      max_height_, &text_layout_));
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
  const auto text_range = text_range_arg.Normalize();

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

Point DWriteTextLayout::TextSinglePoint(Index position, bool trailing) {
  DWRITE_HIT_TEST_METRICS metrics;
  FLOAT left;
  FLOAT top;
  ThrowIfFailed(text_layout_->HitTestTextPosition(static_cast<UINT32>(position),
                                                  static_cast<BOOL>(trailing),
                                                  &left, &top, &metrics));
  return Point{left, top};
}

TextHitTestResult DWriteTextLayout::HitTest(const Point& point) {
  BOOL trailing;
  BOOL inside;
  DWRITE_HIT_TEST_METRICS metrics;

  ThrowIfFailed(text_layout_->HitTestPoint(point.x, point.y, &trailing, &inside,
                                           &metrics));

  TextHitTestResult result;
  result.position = metrics.textPosition;
  result.trailing = trailing != 0;
  result.insideText = inside != 0;
  return result;
}
}  // namespace cru::platform::graphics::win::direct
