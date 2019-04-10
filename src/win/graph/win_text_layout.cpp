#include "cru/win/graph/win_text_layout.hpp"

#include "cru/win/exception.hpp"
#include "cru/win/graph/graph_manager.hpp"
#include "cru/win/graph/win_font.hpp"

#include <cassert>
#include <utility>

namespace cru::win::graph {
WinTextLayout::WinTextLayout(GraphManager* graph_manager,
                             std::shared_ptr<WinFontDescriptor> font,
                             std::wstring text) {
  assert(graph_manager);
  assert(font);
  graph_manager_ = graph_manager;
  text_.swap(text);
  font_descriptor_.swap(font);

  ThrowIfFailed(graph_manager_->GetDWriteFactory()->CreateTextLayout(
      text_.c_str(), static_cast<UINT32>(text_.size()),
      font_descriptor_->GetDWriteTextFormat(), max_width_, max_height_,
      &text_layout_));
}

std::wstring WinTextLayout::GetText() { return text_; }

void WinTextLayout::SetText(std::wstring new_text) {
  text_.swap(new_text);
  ThrowIfFailed(graph_manager_->GetDWriteFactory()->CreateTextLayout(
      text_.c_str(), static_cast<UINT32>(text_.size()),
      font_descriptor_->GetDWriteTextFormat(), max_width_, max_height_,
      &text_layout_));
}

std::shared_ptr<platform::graph::FontDescriptor> WinTextLayout::GetFont() {
  return font_descriptor_;
}

void WinTextLayout::SetFont(
    std::shared_ptr<platform::graph::FontDescriptor> font) {
  auto f = std::dynamic_pointer_cast<WinFontDescriptor>(font);
  assert(f);
  f.swap(font_descriptor_);
  ThrowIfFailed(graph_manager_->GetDWriteFactory()->CreateTextLayout(
      text_.c_str(), static_cast<UINT32>(text_.size()),
      font_descriptor_->GetDWriteTextFormat(), max_width_, max_height_,
      &text_layout_));
}

void WinTextLayout::SetMaxWidth(float max_width) {
  max_width_ = max_width;
  ThrowIfFailed(text_layout_->SetMaxWidth(max_width_));
}

void WinTextLayout::SetMaxHeight(float max_height) {
  max_height_ = max_height;
  ThrowIfFailed(text_layout_->SetMaxHeight(max_height_));
}

ui::Rect WinTextLayout::GetTextBounds() {
  DWRITE_TEXT_METRICS metrics;
  ThrowIfFailed(text_layout_->GetMetrics(&metrics));
  return ui::Rect{metrics.left, metrics.top, metrics.width, metrics.height};
}

std::vector<ui::Rect> WinTextLayout::TextRangeRect(
    const ui::TextRange& text_range) {
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

  std::vector<ui::Rect> result;
  result.reserve(actual_count);

  for (const auto& metrics : hit_test_metrics) {
    result.push_back(ui::Rect{metrics.left, metrics.top,
                              metrics.left + metrics.width,
                              metrics.top + metrics.height});
  }

  return result;
}
}  // namespace cru::win::graph
