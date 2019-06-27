#include "cru/win/graph/direct/text_layout.hpp"

#include "cru/win/graph/direct/exception.hpp"

#include <cassert>
#include <utility>

namespace cru::platform::graph::win::direct {
DWriteTextLayout::DWriteTextLayout(IDirectFactory* factory,
                                   std::shared_ptr<Font> font,
                                   std::wstring text)
    : text_(std::move(text)) {
  assert(factory);
  assert(font);
  assert(IsDirectResource(font.get()));
  factory_ = factory;
  font_ = std::static_pointer_cast<DWriteFont>(font);

  ThrowIfFailed(factory->GetDWriteFactory()->CreateTextLayout(
      text_.c_str(), static_cast<UINT32>(text_.size()),
      font_->GetComInterface(), max_width_, max_height_, &text_layout_));
}

std::wstring DWriteTextLayout::GetText() { return text_; }

void DWriteTextLayout::SetText(std::wstring new_text) {
  text_.swap(new_text);
  ThrowIfFailed(factory_->GetDWriteFactory()->CreateTextLayout(
      text_.c_str(), static_cast<UINT32>(text_.size()),
      font_->GetComInterface(), max_width_, max_height_, &text_layout_));
}

std::shared_ptr<Font> DWriteTextLayout::GetFont() {
  return std::static_pointer_cast<Font>(font_);
}

void DWriteTextLayout::SetFont(std::shared_ptr<Font> font) {
  assert(IsDirectResource(font.get()));
  auto f = std::static_pointer_cast<DWriteFont>(font);

  f.swap(font_);
  ThrowIfFailed(factory_->GetDWriteFactory()->CreateTextLayout(
      text_.c_str(), static_cast<UINT32>(text_.size()),
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

std::vector<Rect> DWriteTextLayout::TextRangeRect(const TextRange& text_range) {
  DWRITE_TEXT_METRICS text_metrics;
  ThrowIfFailed(text_layout_->GetMetrics(&text_metrics));
  const auto metrics_count =
      text_metrics.lineCount * text_metrics.maxBidiReorderingDepth;

  std::vector<DWRITE_HIT_TEST_METRICS> hit_test_metrics(metrics_count);
  UINT32 actual_count;
  text_layout_->HitTestTextRange(text_range.position, text_range.count, 0, 0,
                                 hit_test_metrics.data(), metrics_count,
                                 &actual_count);

  hit_test_metrics.erase(hit_test_metrics.cbegin() + actual_count,
                         hit_test_metrics.cend());

  std::vector<Rect> result;
  result.reserve(actual_count);

  for (const auto& metrics : hit_test_metrics) {
    result.push_back(Rect{metrics.left, metrics.top,
                          metrics.left + metrics.width,
                          metrics.top + metrics.height});
  }

  return result;
}
}  // namespace cru::platform::graph::win::direct
