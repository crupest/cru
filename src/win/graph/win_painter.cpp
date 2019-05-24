#include "cru/win/graph/win_painter.hpp"

#include "cru/win/exception.hpp"
#include "cru/win/graph/win_native_factory.hpp"
#include "cru/win/graph/util/convert_util.hpp"
#include "cru/win/graph/win_brush.hpp"
#include "cru/win/graph/win_geometry.hpp"
#include "cru/win/graph/win_text_layout.hpp"

#include <cassert>

namespace cru::win::graph {
WinPainter::WinPainter(ID2D1RenderTarget* render_target) {
  assert(render_target);
  render_target_ = render_target;
}

platform::Matrix WinPainter::GetTransform() {
  assert(!IsEnded());
  D2D1_MATRIX_3X2_F m;
  render_target_->GetTransform(&m);
  return util::Convert(m);
}

void WinPainter::SetTransform(const platform::Matrix& matrix) {
  assert(!IsEnded());
  render_target_->SetTransform(util::Convert(matrix));
}

void WinPainter::Clear(const ui::Color& color) {
  assert(!IsEnded());
  render_target_->Clear(util::Convert(color));
}

void WinPainter::StrokeRectangle(const ui::Rect& rectangle,
                                 platform::graph::IBrush* brush, float width) {
  assert(!IsEnded());
  const auto b = dynamic_cast<IWinBrush*>(brush);
  assert(b);
  render_target_->DrawRectangle(util::Convert(rectangle), b->GetD2DBrush(),
                                width);
}

void WinPainter::FillRectangle(const ui::Rect& rectangle,
                               platform::graph::IBrush* brush) {
  assert(!IsEnded());
  const auto b = dynamic_cast<IWinBrush*>(brush);
  assert(b);
  render_target_->FillRectangle(util::Convert(rectangle), b->GetD2DBrush());
}

void WinPainter::StrokeGeometry(platform::graph::IGeometry* geometry,
                                platform::graph::IBrush* brush, float width) {
  assert(!IsEnded());
  const auto g = dynamic_cast<WinGeometry*>(geometry);
  assert(g);
  const auto b = dynamic_cast<IWinBrush*>(brush);
  assert(b);

  render_target_->DrawGeometry(g->GetNative(), b->GetD2DBrush(), width);
}

void WinPainter::FillGeometry(platform::graph::IGeometry* geometry,
                              platform::graph::IBrush* brush) {
  assert(!IsEnded());
  const auto g = dynamic_cast<WinGeometry*>(geometry);
  assert(g);
  const auto b = dynamic_cast<IWinBrush*>(brush);
  assert(b);

  render_target_->FillGeometry(g->GetNative(), b->GetD2DBrush());
}

void WinPainter::DrawText(const ui::Point& offset,
                          platform::graph::ITextLayout* text_layout,
                          platform::graph::IBrush* brush) {
  assert(!IsEnded());
  const auto t = dynamic_cast<WinTextLayout*>(text_layout);
  assert(t);
  const auto b = dynamic_cast<IWinBrush*>(brush);
  assert(b);

  render_target_->DrawTextLayout(util::Convert(offset),
                                 t->GetDWriteTextLayout(), b->GetD2DBrush());
}

void WinPainter::End() {
  if (!is_draw_ended_) {
    is_draw_ended_ = true;
    DoEndDraw();
  }
}
}  // namespace cru::win::graph
