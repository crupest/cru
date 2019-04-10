#include "cru/win/graph/win_geometry.hpp"

#include "cru/win/exception.hpp"
#include "cru/win/graph/d2d_util.hpp"
#include "cru/win/graph/graph_manager.hpp"

#include <cassert>

namespace cru::win::graph {
WinGeometryBuilder::WinGeometryBuilder(GraphManager* graph_manager) {
  assert(graph_manager);
  ThrowIfFailed(
      graph_manager->GetD2D1Factory()->CreatePathGeometry(&geometry_));
  ThrowIfFailed(geometry_->Open(&geometry_sink_));
}

WinGeometryBuilder::~WinGeometryBuilder() {
  if (geometry_sink_) {
    ThrowIfFailed(geometry_sink_->Close());
  }
}

void WinGeometryBuilder::BeginFigure(const ui::Point& point) {
  assert(IsValid());
  geometry_sink_->BeginFigure(util::Convert(point), D2D1_FIGURE_BEGIN_FILLED);
}

void WinGeometryBuilder::LineTo(const ui::Point& point) {
  assert(IsValid());
  geometry_sink_->AddLine(util::Convert(point));
}

void WinGeometryBuilder::QuadraticBezierTo(const ui::Point& control_point,
                                           const ui::Point& end_point) {
  assert(IsValid());
  geometry_sink_->AddQuadraticBezier(D2D1::QuadraticBezierSegment(
      util::Convert(control_point), util::Convert(end_point)));
}

void WinGeometryBuilder::CloseFigure(bool close) {
  assert(IsValid());
  geometry_sink_->EndFigure(close ? D2D1_FIGURE_END_CLOSED
                                  : D2D1_FIGURE_END_OPEN);
}

platform::graph::Geometry* WinGeometryBuilder::Build() {
  assert(IsValid());
  ThrowIfFailed(geometry_sink_->Close());
  geometry_sink_ = nullptr;
  const auto geometry = new WinGeometry(geometry_);
  geometry_ = nullptr;
  return geometry;
}

WinGeometry::WinGeometry(Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry) {
  assert(geometry);
  geometry_ = std::move(geometry);
}

bool WinGeometry::FillContains(const ui::Point& point) {
  BOOL result;
  ThrowIfFailed(geometry_->FillContainsPoint(
      util::Convert(point), D2D1::Matrix3x2F::Identity(), &result));
  return result != 0;
}
}  // namespace cru::win::graph
