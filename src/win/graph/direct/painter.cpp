#include "cru/win/graph/direct/painter.hpp"

#include "cru/win/graph/direct/brush.hpp"
#include "cru/win/graph/direct/convert_util.hpp"
#include "cru/win/graph/direct/exception.hpp"
#include "cru/win/graph/direct/geometry.hpp"
#include "cru/win/graph/direct/text_layout.hpp"

#include <cassert>
#include <type_traits>

namespace cru::platform::graph::win::direct {

namespace {
template <typename T, typename U, typename = void>
struct is_static_castable : std::false_type {};

template <typename T, typename U>
struct is_static_castable<
    T, U, std::void_t<decltype(static_cast<U>(std::declval<T>()))>>
    : std::true_type {};

template <typename TDes, typename TSrc>
TDes* CheckAndCast(TSrc* src) {
  assert(src);
  assert(IsDirectResource(src));
  if constexpr (is_static_castable<TSrc*, TDes*>::value)
    return static_cast<TDes*>(src);
  else {
    TDes* d = dynamic_cast<TDes*>(src);
    assert(d);
    return d;
  }
}
}  // namespace

D2DPainter::D2DPainter(ID2D1RenderTarget* render_target) {
  assert(render_target);
  render_target_ = render_target;
}

platform::Matrix D2DPainter::GetTransform() {
  assert(IsValid());
  D2D1_MATRIX_3X2_F m;
  render_target_->GetTransform(&m);
  return Convert(m);
}

void D2DPainter::SetTransform(const platform::Matrix& matrix) {
  assert(IsValid());
  render_target_->SetTransform(Convert(matrix));
}

void D2DPainter::Clear(const Color& color) {
  assert(IsValid());
  render_target_->Clear(Convert(color));
}

void D2DPainter::StrokeRectangle(const Rect& rectangle, Brush* brush,
                                 float width) {
  assert(IsValid());
  const auto b = CheckAndCast<ID2DBrush>(brush);
  render_target_->DrawRectangle(Convert(rectangle), b->GetD2DBrushInterface(),
                                width);
}

void D2DPainter::FillRectangle(const Rect& rectangle, Brush* brush) {
  assert(IsValid());
  const auto b = CheckAndCast<ID2DBrush>(brush);
  render_target_->FillRectangle(Convert(rectangle), b->GetD2DBrushInterface());
}

void D2DPainter::StrokeGeometry(Geometry* geometry, Brush* brush, float width) {
  assert(IsValid());
  const auto g = CheckAndCast<D2DGeometry>(geometry);
  const auto b = CheckAndCast<ID2DBrush>(brush);

  render_target_->DrawGeometry(g->GetComInterface(), b->GetD2DBrushInterface(),
                               width);
}

void D2DPainter::FillGeometry(Geometry* geometry, Brush* brush) {
  assert(IsValid());
  const auto g = CheckAndCast<D2DGeometry>(geometry);
  const auto b = CheckAndCast<ID2DBrush>(brush);

  render_target_->FillGeometry(g->GetComInterface(), b->GetD2DBrushInterface());
}

void D2DPainter::DrawText(const Point& offset, TextLayout* text_layout,
                          Brush* brush) {
  assert(IsValid());
  const auto t = CheckAndCast<DWriteTextLayout>(text_layout);
  const auto b = CheckAndCast<ID2DBrush>(brush);

  render_target_->DrawTextLayout(Convert(offset), t->GetComInterface(),
                                 b->GetD2DBrushInterface());
}

void D2DPainter::EndDraw() {
  if (is_drawing_) {
    is_drawing_ = false;
    DoEndDraw();
  }
}
}  // namespace cru::platform::graph::win::direct
