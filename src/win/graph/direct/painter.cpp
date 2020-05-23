#include "cru/win/graph/direct/painter.hpp"

#include "cru/platform/check.hpp"
#include "cru/win/graph/direct/brush.hpp"
#include "cru/win/graph/direct/convert_util.hpp"
#include "cru/win/graph/direct/exception.hpp"
#include "cru/win/graph/direct/geometry.hpp"
#include "cru/win/graph/direct/text_layout.hpp"

#include <type_traits>

namespace cru::platform::graph::win::direct {
D2DPainter::D2DPainter(ID2D1RenderTarget* render_target) {
  Expects(render_target);
  render_target_ = render_target;
}

platform::Matrix D2DPainter::GetTransform() {
  CheckValidation();
  D2D1_MATRIX_3X2_F m;
  render_target_->GetTransform(&m);
  return Convert(m);
}

void D2DPainter::SetTransform(const platform::Matrix& matrix) {
  CheckValidation();
  render_target_->SetTransform(Convert(matrix));
}

void D2DPainter::Clear(const Color& color) {
  CheckValidation();
  render_target_->Clear(Convert(color));
}

void D2DPainter::StrokeRectangle(const Rect& rectangle, IBrush* brush,
                                 float width) {
  CheckValidation();
  const auto b = CheckPlatform<ID2DBrush>(brush, GetPlatformId());
  render_target_->DrawRectangle(Convert(rectangle), b->GetD2DBrushInterface(),
                                width);
}

void D2DPainter::FillRectangle(const Rect& rectangle, IBrush* brush) {
  CheckValidation();
  const auto b = CheckPlatform<ID2DBrush>(brush, GetPlatformId());
  render_target_->FillRectangle(Convert(rectangle), b->GetD2DBrushInterface());
}

void D2DPainter::StrokeGeometry(IGeometry* geometry, IBrush* brush,
                                float width) {
  CheckValidation();
  const auto g = CheckPlatform<D2DGeometry>(geometry, GetPlatformId());
  const auto b = CheckPlatform<ID2DBrush>(brush, GetPlatformId());
  render_target_->DrawGeometry(g->GetComInterface(), b->GetD2DBrushInterface(),
                               width);
}

void D2DPainter::FillGeometry(IGeometry* geometry, IBrush* brush) {
  CheckValidation();
  const auto g = CheckPlatform<D2DGeometry>(geometry, GetPlatformId());
  const auto b = CheckPlatform<ID2DBrush>(brush, GetPlatformId());
  render_target_->FillGeometry(g->GetComInterface(), b->GetD2DBrushInterface());
}

void D2DPainter::DrawText(const Point& offset, ITextLayout* text_layout,
                          IBrush* brush) {
  CheckValidation();
  const auto t = CheckPlatform<DWriteTextLayout>(text_layout, GetPlatformId());
  const auto b = CheckPlatform<ID2DBrush>(brush, GetPlatformId());
  render_target_->DrawTextLayout(Convert(offset), t->GetComInterface(),
                                 b->GetD2DBrushInterface());
}

void D2DPainter::PushLayer(const Rect& bounds) {
  CheckValidation();

  Microsoft::WRL::ComPtr<ID2D1Layer> layer;
  ThrowIfFailed(render_target_->CreateLayer(&layer));

  render_target_->PushLayer(D2D1::LayerParameters(Convert(bounds)),
                            layer.Get());

  layers_.push_back(std::move(layer));
}

void D2DPainter::PopLayer() {
  render_target_->PopLayer();
  layers_.pop_back();
}

void D2DPainter::EndDraw() {
  if (is_drawing_) {
    is_drawing_ = false;
    DoEndDraw();
  }
}

void D2DPainter::CheckValidation() {
  if (!is_drawing_) {
    throw cru::platform::ReuseException(
        "Can't do that on painter after end drawing.");
  }
}
}  // namespace cru::platform::graph::win::direct
