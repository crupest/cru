#include "text_render_object.hpp"

#include <cassert>

#include "exception.hpp"
#include "graph/graph.hpp"

namespace cru::ui::render {
TextRenderObject::TextRenderObject(
    Microsoft::WRL::ComPtr<ID2D1Brush> brush,
    Microsoft::WRL::ComPtr<IDWriteTextFormat> format,
    Microsoft::WRL::ComPtr<ID2D1Brush> selection_brush)
    : brush_(std::move(brush)),
      text_format_(std::move(format)),
      selection_brush_(std::move(selection_brush)) {
  RecreateTextLayout();
}

namespace {
void DrawSelectionRect(ID2D1RenderTarget* render_target,
                       IDWriteTextLayout* layout, ID2D1Brush* brush,
                       const std::optional<TextRange> range) {
  if (range.has_value()) {
    DWRITE_TEXT_METRICS text_metrics{};
    ThrowIfFailed(layout->GetMetrics(&text_metrics));
    const auto metrics_count =
        text_metrics.lineCount * text_metrics.maxBidiReorderingDepth;

    std::vector<DWRITE_HIT_TEST_METRICS> hit_test_metrics(metrics_count);
    UINT32 actual_count;
    layout->HitTestTextRange(range.value().position, range.value().count, 0, 0,
                             hit_test_metrics.data(), metrics_count,
                             &actual_count);

    hit_test_metrics.erase(hit_test_metrics.cbegin() + actual_count,
                           hit_test_metrics.cend());

    for (const auto& metrics : hit_test_metrics)
      render_target->FillRoundedRectangle(
          D2D1::RoundedRect(D2D1::RectF(metrics.left, metrics.top,
                                        metrics.left + metrics.width,
                                        metrics.top + metrics.height),
                            3, 3),
          brush);
  }
}
}  // namespace

void TextRenderObject::Draw(ID2D1RenderTarget* render_target) {
  graph::WithTransform(
      render_target,
      D2D1::Matrix3x2F::Translation(GetMargin().left + GetPadding().left,
                                    GetMargin().top + GetPadding().top),
      [this](auto rt) {
        DrawSelectionRect(rt, text_layout_.Get(), selection_brush_.Get(),
                          selection_range_);
        rt->DrawTextLayout(D2D1::Point2F(), text_layout_.Get(), brush_.Get());
      });
}

RenderObject* TextRenderObject::HitTest(const Point& point) {
  return Rect{Point::Zero(), GetSize()}.IsPointInside(point) ? this : nullptr;
}

void TextRenderObject::OnSizeChanged(const Size& old_size,
                                     const Size& new_size) {
  ThrowIfFailed(text_layout_->SetMaxWidth(new_size.width));
  ThrowIfFailed(text_layout_->SetMaxHeight(new_size.height));
}

Size TextRenderObject::OnMeasureContent(const Size& available_size) {
  ThrowIfFailed(text_layout_->SetMaxWidth(available_size.width));
  ThrowIfFailed(text_layout_->SetMaxHeight(available_size.height));

  DWRITE_TEXT_METRICS metrics;
  ThrowIfFailed(text_layout_->GetMetrics(&metrics));

  return Size(metrics.width, metrics.height);
}

void TextRenderObject::OnLayoutContent(const Rect& content_rect) {}

void TextRenderObject::RecreateTextLayout() {
  assert(text_format_ != nullptr);

  text_layout_ = nullptr;  // release last one

  const auto dwrite_factory =
      graph::GraphManager::GetInstance()->GetDWriteFactory();

  const auto&& size = GetSize();

  ThrowIfFailed(dwrite_factory->CreateTextLayout(
      text_.c_str(), static_cast<UINT32>(text_.size()), text_format_.Get(),
      size.width, size.height, &text_layout_));
}
}  // namespace cru::ui::render
