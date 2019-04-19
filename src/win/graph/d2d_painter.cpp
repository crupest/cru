#include "cru/win/graph/d2d_painter.hpp"

#include "cru/win/exception.hpp"
#include "cru/win/graph/d2d_util.hpp"
#include "cru/win/graph/graph_manager.hpp"
#include "cru/win/graph/win_brush.hpp"
#include "cru/win/graph/win_geometry.hpp"
#include "cru/win/graph/win_text_layout.hpp"

#include <cassert>

namespace cru::win::graph {
D2DPainter::D2DPainter(ID2D1RenderTarget* render_target) {
  assert(render_target);
  render_target_ = render_target;
}

platform::Matrix D2DPainter::GetTransform() {
  assert(!IsDisposed());
  D2D1_MATRIX_3X2_F m;
  render_target_->GetTransform(&m);
  return util::Convert(m);
}

void D2DPainter::SetTransform(const platform::Matrix& matrix) {
  assert(!IsDisposed());
  render_target_->SetTransform(util::Convert(matrix));
}

void D2DPainter::Clear(const ui::Color& color) {
  assert(!IsDisposed());
  render_target_->Clear(util::Convert(color));
}

void D2DPainter::StrokeRectangle(const ui::Rect& rectangle,
                                 platform::graph::Brush* brush, float width) {
  assert(!IsDisposed());
  const auto b = dynamic_cast<WinBrush*>(brush);
  assert(b);
  render_target_->DrawRectangle(util::Convert(rectangle), b->GetD2DBrush(),
                                width);
}

void D2DPainter::FillRectangle(const ui::Rect& rectangle,
                               platform::graph::Brush* brush) {
  assert(!IsDisposed());
  const auto b = dynamic_cast<WinBrush*>(brush);
  assert(b);
  render_target_->FillRectangle(util::Convert(rectangle), b->GetD2DBrush());
}

void D2DPainter::StrokeGeometry(platform::graph::Geometry* geometry,
                                platform::graph::Brush* brush, float width) {
  assert(!IsDisposed());
  const auto g = dynamic_cast<WinGeometry*>(geometry);
  assert(g);
  const auto b = dynamic_cast<WinBrush*>(brush);
  assert(b);

  render_target_->DrawGeometry(g->GetNative(), b->GetD2DBrush(), width);
}

void D2DPainter::FillGeometry(platform::graph::Geometry* geometry,
                              platform::graph::Brush* brush) {
  assert(!IsDisposed());
  const auto g = dynamic_cast<WinGeometry*>(geometry);
  assert(g);
  const auto b = dynamic_cast<WinBrush*>(brush);
  assert(b);

  render_target_->FillGeometry(g->GetNative(), b->GetD2DBrush());
}

void D2DPainter::DrawText(const ui::Point& offset,
                          platform::graph::TextLayout* text_layout,
                          platform::graph::Brush* brush) {
  assert(!IsDisposed());
  const auto t = dynamic_cast<WinTextLayout*>(text_layout);
  assert(t);
  const auto b = dynamic_cast<WinBrush*>(brush);
  assert(b);

  render_target_->DrawTextLayout(util::Convert(offset),
                                 t->GetDWriteTextLayout(), b->GetD2DBrush());
}

void D2DPainter::EndDraw() {
  if (!is_disposed_) {
    is_disposed_ = true;
    DoEndDraw();
  }
}
}  // namespace cru::win::graph
