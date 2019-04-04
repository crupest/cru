#include "cru/platform/win/win_painter.hpp"

#include "cru/platform/win/d2d_util.hpp"
#include "cru/platform/win/exception.hpp"
#include "cru/platform/win/graph_manager.hpp"
#include "cru/platform/win/win_brush.hpp"
#include "cru/platform/win/win_geometry.hpp"
#include "cru/platform/win/win_native_window.hpp"
#include "cru/platform/win/win_text_layout.hpp"
#include "cru/platform/win/window_render_target.hpp"

#include <cassert>

namespace cru::platform::win {
WinPainter::WinPainter(WinNativeWindow* window) {
  assert(window);
  window_ = window;
  const auto window_render_target = window_->GetWindowRenderTarget();
  render_target_ =
      window_render_target->GetGraphManager()->GetD2D1DeviceContext();
  window_render_target->SetAsTarget();
  render_target_->BeginDraw();
}

WinPainter::~WinPainter() {
  if (!IsDisposed()) {
    ThrowIfFailed(render_target_->EndDraw());
  }
}

Matrix WinPainter::GetTransform() {
  assert(!IsDisposed());
  D2D1_MATRIX_3X2_F m;
  render_target_->GetTransform(&m);
  return util::Convert(m);
}

void WinPainter::SetTransform(const Matrix& matrix) {
  assert(!IsDisposed());
  render_target_->SetTransform(util::Convert(matrix));
}

void WinPainter::Clear(const ui::Color& color) {
  assert(!IsDisposed());
  render_target_->Clear(util::Convert(color));
}

void WinPainter::StrokeRectangle(const ui::Rect& rectangle, Brush* brush,
                                 float width) {
  assert(!IsDisposed());
  const auto b = dynamic_cast<WinBrush*>(brush);
  assert(b);
  render_target_->DrawRectangle(util::Convert(rectangle), b->GetD2DBrush(),
                                width);
}

void WinPainter::FillRectangle(const ui::Rect& rectangle, Brush* brush) {
  assert(!IsDisposed());
  const auto b = dynamic_cast<WinBrush*>(brush);
  assert(b);
  render_target_->FillRectangle(util::Convert(rectangle), b->GetD2DBrush());
}

void WinPainter::StrokeGeometry(Geometry* geometry, Brush* brush, float width) {
  assert(!IsDisposed());
  const auto g = dynamic_cast<WinGeometry*>(geometry);
  assert(g);
  const auto b = dynamic_cast<WinBrush*>(brush);
  assert(b);

  render_target_->DrawGeometry(g->GetNative(), b->GetD2DBrush(), width);
}

void WinPainter::FillGeometry(Geometry* geometry, Brush* brush) {
  assert(!IsDisposed());
  const auto g = dynamic_cast<WinGeometry*>(geometry);
  assert(g);
  const auto b = dynamic_cast<WinBrush*>(brush);
  assert(b);

  render_target_->FillGeometry(g->GetNative(), b->GetD2DBrush());
}

void WinPainter::DrawText(const ui::Point& offset, TextLayout* text_layout,
                          Brush* brush) {
  assert(!IsDisposed());
  const auto t = dynamic_cast<WinTextLayout*>(text_layout);
  assert(t);
  const auto b = dynamic_cast<WinBrush*>(brush);
  assert(b);

  render_target_->DrawTextLayout(util::Convert(offset),
                                 t->GetDWriteTextLayout(), b->GetD2DBrush());
}

void WinPainter::EndDraw() {
  if (!IsDisposed()) {
    ThrowIfFailed(render_target_->EndDraw());
    render_target_ = nullptr;
    is_disposed = true;
  }
}
}  // namespace cru::platform::win
