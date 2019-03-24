#include "text_render_object.hpp"

#include <d2d1.h>
#include <dwrite.h>
#include <algorithm>

#include "exception.hpp"
#include "graph/graph_manager.hpp"
#include "graph/graph_util.hpp"
#include "util/com_util.hpp"

namespace cru::ui::render {
TextRenderObject::TextRenderObject(ID2D1Brush* brush, IDWriteTextFormat* format,
                                   ID2D1Brush* selection_brush) {
  assert(brush);
  assert(format);
  assert(selection_brush);
  brush->AddRef();
  format->AddRef();
  selection_brush->AddRef();
  this->brush_ = brush;
  this->text_format_ = format;
  this->selection_brush_ = selection_brush;
  try {
    RecreateTextLayout();
  } catch (...) {
    brush->Release();
    format->Release();
    selection_brush->Release();
    throw;
  }
}

TextRenderObject::~TextRenderObject() {
  util::SafeRelease(brush_);
  util::SafeRelease(text_format_);
  util::SafeRelease(text_layout_);
  util::SafeRelease(selection_brush_);
}

void TextRenderObject::SetBrush(ID2D1Brush* new_brush) {
  assert(new_brush);
  util::SafeRelease(brush_);
  new_brush->AddRef();
  brush_ = new_brush;
}

void TextRenderObject::SetTextFormat(IDWriteTextFormat* new_text_format) {
  assert(new_text_format);
  util::SafeRelease(text_format_);
  new_text_format->AddRef();
  text_format_ = new_text_format;
  RecreateTextLayout();
}

void TextRenderObject::SetSelectionBrush(ID2D1Brush* new_brush) {
  assert(new_brush);
  util::SafeRelease(selection_brush_);
  new_brush->AddRef();
  selection_brush_ = new_brush;
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
        DrawSelectionRect(rt, text_layout_, selection_brush_, selection_range_);
        rt->DrawTextLayout(D2D1::Point2F(), text_layout_, brush_);
      });
}

RenderObject* TextRenderObject::HitTest(const Point& point) {
  const auto margin = GetMargin();
  const auto size = GetSize();
  return Rect{margin.left, margin.top,
              std::max(size.width - margin.GetHorizontalTotal(), 0.0f),
              std::max(size.height - margin.GetVerticalTotal(), 0.0f)}
                 .IsPointInside(point)
             ? this
             : nullptr;
}

void TextRenderObject::OnSizeChanged(const Size& old_size,
                                     const Size& new_size) {
  const auto margin = GetMargin();
  const auto padding = GetPadding();
  ThrowIfFailed(text_layout_->SetMaxWidth(
      std::max(new_size.width - margin.GetHorizontalTotal() -
                   padding.GetHorizontalTotal(),
               0.0f)));
  ThrowIfFailed(text_layout_->SetMaxHeight(std::max(
      new_size.height - margin.GetVerticalTotal() - padding.GetVerticalTotal(),
      0.0f)));
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

  util::SafeRelease(text_layout_);

  const auto dwrite_factory =
      graph::GraphManager::GetInstance()->GetDWriteFactory();

  const auto&& size = GetSize();

  ThrowIfFailed(dwrite_factory->CreateTextLayout(
      text_.c_str(), static_cast<UINT32>(text_.size()), text_format_,
      size.width, size.height, &text_layout_));
}
}  // namespace cru::ui::render
