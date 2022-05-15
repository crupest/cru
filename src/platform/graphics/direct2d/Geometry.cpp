#include "cru/platform/graphics/direct2d/Geometry.h"

#include "cru/common/platform/win/Exception.h"
#include "cru/platform/graphics/direct2d/ConvertUtil.h"
#include "cru/platform/graphics/direct2d/Exception.h"
#include "cru/platform/graphics/direct2d/Factory.h"

#include <d2d1.h>
#include <d2d1helper.h>

namespace cru::platform::graphics::direct2d {
D2DGeometryBuilder::D2DGeometryBuilder(DirectGraphicsFactory* factory)
    : DirectGraphicsResource(factory) {
  ThrowIfFailed(factory->GetD2D1Factory()->CreatePathGeometry(&geometry_));
  ThrowIfFailed(geometry_->Open(&geometry_sink_));
}

void D2DGeometryBuilder::CheckValidation() {
  if (!IsValid())
    throw ReuseException(u"The geometry builder is already disposed.");
}

Point D2DGeometryBuilder::GetCurrentPosition() {
  CheckValidation();
  return current_position_;
}

void D2DGeometryBuilder::MoveTo(const Point& point) {
  CheckValidation();
  geometry_sink_->BeginFigure(Convert(point), D2D1_FIGURE_BEGIN_FILLED);
  start_point_ = current_position_ = point;
}

void D2DGeometryBuilder::LineTo(const Point& point) {
  CheckValidation();
  geometry_sink_->AddLine(Convert(point));
  current_position_ = point;
}

void D2DGeometryBuilder::CubicBezierTo(const Point& start_control_point,
                                       const Point& end_control_point,
                                       const Point& end_point) {
  CheckValidation();
  geometry_sink_->AddBezier(D2D1::BezierSegment(Convert(start_control_point),
                                                Convert(end_control_point),
                                                Convert(end_point)));
  current_position_ = end_point;
}

void D2DGeometryBuilder::QuadraticBezierTo(const Point& control_point,
                                           const Point& end_point) {
  CheckValidation();
  geometry_sink_->AddQuadraticBezier(
      D2D1::QuadraticBezierSegment(Convert(control_point), Convert(end_point)));
  current_position_ = end_point;
}

void D2DGeometryBuilder::ArcTo(const Point& radius, float angle,
                               bool is_large_arc, bool is_clockwise,
                               const Point& end_point) {
  CheckValidation();
  geometry_sink_->AddArc(D2D1::ArcSegment(
      Convert(end_point), {radius.x, radius.y}, angle,
      is_clockwise ? D2D1_SWEEP_DIRECTION_CLOCKWISE
                   : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
      is_large_arc ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL));
  current_position_ = end_point;
}

void D2DGeometryBuilder::CloseFigure(bool close) {
  CheckValidation();
  geometry_sink_->EndFigure(close ? D2D1_FIGURE_END_CLOSED
                                  : D2D1_FIGURE_END_OPEN);
  current_position_ = start_point_;
}

std::unique_ptr<IGeometry> D2DGeometryBuilder::Build() {
  CheckValidation();
  ThrowIfFailed(geometry_sink_->Close());
  geometry_sink_ = nullptr;
  auto geometry =
      std::make_unique<D2DGeometry>(GetDirectFactory(), std::move(geometry_));
  geometry_ = nullptr;
  return geometry;
}

D2DGeometry::D2DGeometry(DirectGraphicsFactory* factory,
                         Microsoft::WRL::ComPtr<ID2D1Geometry> geometry)
    : DirectGraphicsResource(factory), geometry_(std::move(geometry)) {}

bool D2DGeometry::FillContains(const Point& point) {
  BOOL result;
  ThrowIfFailed(geometry_->FillContainsPoint(
      Convert(point), D2D1::Matrix3x2F::Identity(), &result));
  return result != 0;
}

Rect D2DGeometry::GetBounds() {
  D2D1_RECT_F bounds;
  ThrowIfFailed(geometry_->GetBounds(D2D1::Matrix3x2F::Identity(), &bounds));
  return Convert(bounds);
}

std::unique_ptr<IGeometry> D2DGeometry::Transform(const Matrix& matrix) {
  Microsoft::WRL::ComPtr<ID2D1TransformedGeometry> d2d1_geometry;
  ThrowIfFailed(GetDirectFactory()->GetD2D1Factory()->CreateTransformedGeometry(
      geometry_.Get(), Convert(matrix), &d2d1_geometry));
  return std::make_unique<D2DGeometry>(GetDirectFactory(),
                                       std::move(d2d1_geometry));
}

std::unique_ptr<IGeometry> D2DGeometry::CreateStrokeGeometry(float width) {
  Microsoft::WRL::ComPtr<ID2D1PathGeometry> d2d1_geometry;
  ThrowIfFailed(
      GetDirectFactory()->GetD2D1Factory()->CreatePathGeometry(&d2d1_geometry));
  Microsoft::WRL::ComPtr<ID2D1GeometrySink> d2d1_geometry_sink;
  ThrowIfFailed(d2d1_geometry->Open(&d2d1_geometry_sink));
  ThrowIfFailed(
      geometry_->Widen(width, nullptr, nullptr, d2d1_geometry_sink.Get()));
  d2d1_geometry_sink->Close();

  return std::make_unique<D2DGeometry>(GetDirectFactory(),
                                       std::move(d2d1_geometry));
}
}  // namespace cru::platform::graphics::direct2d
