#include "cru/win/graph/direct/geometry.hpp"

#include "cru/win/graph/direct/convert_util.hpp"
#include "cru/win/graph/direct/exception.hpp"

#include <cassert>

namespace cru::platform::graph::win::direct {
D2DGeometryBuilder::D2DGeometryBuilder(IDirectFactory* factory) {
  assert(factory);
  ThrowIfFailed(factory->GetD2D1Factory()->CreatePathGeometry(&geometry_));
  ThrowIfFailed(geometry_->Open(&geometry_sink_));
}

D2DGeometryBuilder::~D2DGeometryBuilder() {
  if (geometry_sink_) {
    ThrowIfFailed(geometry_sink_->Close());
  }
}

void D2DGeometryBuilder::BeginFigure(const Point& point) {
  assert(IsValid());
  geometry_sink_->BeginFigure(Convert(point), D2D1_FIGURE_BEGIN_FILLED);
}

void D2DGeometryBuilder::LineTo(const Point& point) {
  assert(IsValid());
  geometry_sink_->AddLine(Convert(point));
}

void D2DGeometryBuilder::QuadraticBezierTo(const Point& control_point,
                                           const Point& end_point) {
  assert(IsValid());
  geometry_sink_->AddQuadraticBezier(
      D2D1::QuadraticBezierSegment(Convert(control_point), Convert(end_point)));
}

void D2DGeometryBuilder::CloseFigure(bool close) {
  assert(IsValid());
  geometry_sink_->EndFigure(close ? D2D1_FIGURE_END_CLOSED
                                  : D2D1_FIGURE_END_OPEN);
}

Geometry* D2DGeometryBuilder::Build() {
  assert(IsValid());
  ThrowIfFailed(geometry_sink_->Close());
  geometry_sink_ = nullptr;
  const auto geometry = new D2DGeometry(std::move(geometry_));
  geometry_ = nullptr;
  return geometry;
}

D2DGeometry::D2DGeometry(Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry) {
  assert(geometry);
  geometry_ = std::move(geometry);
}

bool D2DGeometry::FillContains(const Point& point) {
  BOOL result;
  ThrowIfFailed(geometry_->FillContainsPoint(
      Convert(point), D2D1::Matrix3x2F::Identity(), &result));
  return result != 0;
}
}  // namespace cru::platform::graph::win::direct
