#include "cru/win/graphics/direct/Geometry.hpp"

#include "cru/win/graphics/direct/ConvertUtil.hpp"
#include "cru/win/graphics/direct/Exception.hpp"
#include "cru/win/graphics/direct/Factory.hpp"

namespace cru::platform::graphics::win::direct {
D2DGeometryBuilder::D2DGeometryBuilder(DirectGraphFactory* factory)
    : DirectGraphResource(factory) {
  ThrowIfFailed(factory->GetD2D1Factory()->CreatePathGeometry(&geometry_));
  ThrowIfFailed(geometry_->Open(&geometry_sink_));
}

void D2DGeometryBuilder::CheckValidation() {
  if (!IsValid())
    throw ReuseException("The geometry builder is already disposed.");
}

void D2DGeometryBuilder::BeginFigure(const Point& point) {
  CheckValidation();
  geometry_sink_->BeginFigure(Convert(point), D2D1_FIGURE_BEGIN_FILLED);
}

void D2DGeometryBuilder::LineTo(const Point& point) {
  CheckValidation();
  geometry_sink_->AddLine(Convert(point));
}

void D2DGeometryBuilder::QuadraticBezierTo(const Point& control_point,
                                           const Point& end_point) {
  CheckValidation();
  geometry_sink_->AddQuadraticBezier(
      D2D1::QuadraticBezierSegment(Convert(control_point), Convert(end_point)));
}

void D2DGeometryBuilder::CloseFigure(bool close) {
  CheckValidation();
  geometry_sink_->EndFigure(close ? D2D1_FIGURE_END_CLOSED
                                  : D2D1_FIGURE_END_OPEN);
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

D2DGeometry::D2DGeometry(DirectGraphFactory* factory,
                         Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry)
    : DirectGraphResource(factory), geometry_(std::move(geometry)) {}

bool D2DGeometry::FillContains(const Point& point) {
  BOOL result;
  ThrowIfFailed(geometry_->FillContainsPoint(
      Convert(point), D2D1::Matrix3x2F::Identity(), &result));
  return result != 0;
}
}  // namespace cru::platform::graphics::win::direct
