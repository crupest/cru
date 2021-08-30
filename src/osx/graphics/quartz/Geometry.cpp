#include "cru/osx/graphics/quartz/Geometry.hpp"

#include <memory>

namespace cru::platform::graphics::osx::quartz {
QuartzGeometry::QuartzGeometry(IGraphFactory *graphics_factory,
                               CGPathRef cg_path)
    : OsxQuartzResource(graphics_factory), cg_path_(cg_path) {}

QuartzGeometry::~QuartzGeometry() { CGPathRelease(cg_path_); }

bool QuartzGeometry::FillContains(const Point &point) {
  return CGPathContainsPoint(cg_path_, nullptr, CGPoint{point.x, point.y},
                             kCGPathFill);
}

QuartzGeometryBuilder::QuartzGeometryBuilder(IGraphFactory *graphics_factory)
    : OsxQuartzResource(graphics_factory) {
  cg_mutable_path_ = CGPathCreateMutable();
}

QuartzGeometryBuilder::~QuartzGeometryBuilder() {
  CGPathRelease(cg_mutable_path_);
}

void QuartzGeometryBuilder::BeginFigure(const Point &point) {
  CGPathMoveToPoint(cg_mutable_path_, nullptr, point.x, point.y);
}

void QuartzGeometryBuilder::CloseFigure(bool close) {
  if (close) CGPathCloseSubpath(cg_mutable_path_);
}

void QuartzGeometryBuilder::LineTo(const Point &point) {
  CGPathAddLineToPoint(cg_mutable_path_, nullptr, point.x, point.y);
}

void QuartzGeometryBuilder::QuadraticBezierTo(const Point &control_point,
                                              const Point &end_point) {
  CGPathAddQuadCurveToPoint(cg_mutable_path_, nullptr, control_point.x,
                            control_point.y, end_point.x, end_point.y);
}

std::unique_ptr<IGeometry> QuartzGeometryBuilder::Build() {
  return std::make_unique<QuartzGeometry>(GetGraphFactory(),
                                          CGPathCreateCopy(cg_mutable_path_));
}
}  // namespace cru::platform::graphics::osx::quartz
