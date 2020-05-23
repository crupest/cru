#include "cru/win/graph/direct/TextLayout.hpp"

#include "cru/common/Logger.hpp"
#include "cru/platform/Check.hpp"
#include "cru/win/graph/direct/Exception.hpp"
#include "cru/win/graph/direct/Factory.hpp"
#include "cru/win/graph/direct/Font.hpp"
#include "cru/win/String.hpp"

#include <utility>

namespace cru::platform::graph::win::direct {
using cru::platform::win::IndexUtf16ToUtf8;
using cru::platform::win::IndexUtf8ToUtf16;

DWriteTextLayout::DWriteTextLayout(DirectGraphFactory* factory,
                                   std::shared_ptr<IFont> font,
                                   std::string text)
    : DirectGraphResource(factory), text_(std::move(text)) {
  Expects(font);
  font_ = CheckPlatform<DWriteFont>(font, GetPlatformId());

  w_text_ = cru::platform::win::ToUtf16String(text_);

  ThrowIfFailed(factory->GetDWriteFactory()->CreateTextLayout(
      w_text_.c_str(), static_cast<UINT32>(w_text_.size()),
      font_->GetComInterface(), max_width_, max_height_, &text_layout_));
}

DWriteTextLayout::~DWriteTextLayout() = default;

std::string DWriteTextLayout::GetText() { return text_; }

void DWriteTextLayout::SetText(std::string new_text) {
  text_.swap(new_text);
  w_text_ = cru::platform::win::ToUtf16String(text_);
  ThrowIfFailed(GetDirectFactory()->GetDWriteFactory()->CreateTextLayout(
      w_text_.c_str(), static_cast<UINT32>(w_text_.size()),
      font_->GetComInterface(), max_width_, max_height_, &text_layout_));
}

std::shared_ptr<IFont> DWriteTextLayout::GetFont() {
  return std::dynamic_pointer_cast<IFont>(font_);
}

void DWriteTextLayout::SetFont(std::shared_ptr<IFont> font) {
  font_ = CheckPlatform<DWriteFont>(font, GetPlatformId());
  ThrowIfFailed(GetDirectFactory()->GetDWriteFactory()->CreateTextLayout(
      w_text_.c_str(), static_cast<UINT32>(w_text_.size()),
      font_->GetComInterface(), max_width_, max_height_, &text_layout_));
}

void DWriteTextLayout::SetMaxWidth(float max_width) {
  max_width_ = max_width;
  ThrowIfFailed(text_layout_->SetMaxWidth(max_width_));
}

void DWriteTextLayout::SetMaxHeight(float max_height) {
  max_height_ = max_height;
  ThrowIfFailed(text_layout_->SetMaxHeight(max_height_));
}

Rect DWriteTextLayout::GetTextBounds() {
  DWRITE_TEXT_METRICS metrics;
  ThrowIfFailed(text_layout_->GetMetrics(&metrics));
  return Rect{metrics.left, metrics.top, metrics.width, metrics.height};
}

std::vector<Rect> DWriteTextLayout::TextRangeRect(
    const TextRange& text_range_arg) {
  if (text_range_arg.count == 0) {
    return {};
  }
  const auto text_range = text_range_arg.Normalize();

  // TODO: This can be faster with one iteration.
  const int start_index =
      IndexUtf8ToUtf16(text_, static_cast<int>(text_range.position), w_text_);
  const int end_index = IndexUtf8ToUtf16(
      text_, static_cast<int>(text_range.position + text_range.count), w_text_);

  DWRITE_TEXT_METRICS text_metrics;
  ThrowIfFailed(text_layout_->GetMetrics(&text_metrics));
  const auto metrics_count =
      text_metrics.lineCount * text_metrics.maxBidiReorderingDepth;

  std::vector<DWRITE_HIT_TEST_METRICS> hit_test_metrics(metrics_count);
  UINT32 actual_count;
  ThrowIfFailed(text_layout_->HitTestTextRange(
      static_cast<UINT32>(start_index),
      static_cast<UINT32>(end_index - start_index), 0, 0,
      hit_test_metrics.data(), metrics_count, &actual_count));

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

Point DWriteTextLayout::TextSinglePoint(gsl::index position, bool trailing) {
  const auto index =
      IndexUtf8ToUtf16(text_, static_cast<int>(position), w_text_);

  DWRITE_HIT_TEST_METRICS metrics;
  FLOAT left;
  FLOAT top;
  ThrowIfFailed(text_layout_->HitTestTextPosition(static_cast<UINT32>(index),
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

  const auto index =
      IndexUtf16ToUtf8(w_text_, static_cast<int>(metrics.textPosition), text_);
  TextHitTestResult result;
  result.position = index;
  result.trailing = trailing != 0;
  result.insideText = inside != 0;
  return result;
}
}  // namespace cru::platform::graph::win::direct
