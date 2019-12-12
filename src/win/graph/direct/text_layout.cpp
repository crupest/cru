#include "cru/win/graph/direct/text_layout.hpp"

#include "cru/platform/check.hpp"
#include "cru/win/graph/direct/exception.hpp"
#include "cru/win/graph/direct/factory.hpp"
#include "cru/win/graph/direct/font.hpp"
#include "cru/win/string.hpp"

#include <cassert>
#include <utility>

namespace cru::platform::graph::win::direct {
DWriteTextLayout::DWriteTextLayout(DirectGraphFactory* factory,
                                   std::shared_ptr<IFont> font,
                                   std::string text)
    : DirectGraphResource(factory), text_(std::move(text)) {
  assert(font);
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
