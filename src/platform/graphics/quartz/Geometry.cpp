#include "cru/platform/graphics/quartz/Geometry.h"
#include "cru/platform/graphics/quartz/Convert.h"

#include <memory>

namespace cru::platform::graphics::quartz {
QuartzGeometry::QuartzGeometry(IGraphicsFactory *graphics_factory,
                               CGPathRef cg_path)
    : OsxQuartzResource(graphics_factory), cg_path_(cg_path) {}

QuartzGeometry::~QuartzGeometry() { CGPathRelease(cg_path_); }

bool QuartzGeometry::FillContains(const Point &point) {
  return CGPathContainsPoint(cg_path_, nullptr, CGPoint{point.x, point.y},
                             kCGPathFill);
}

Rect QuartzGeometry::GetBounds() {
  auto bounds = CGPathGetPathBoundingBox(cg_path_);
  if (CGRectIsNull(bounds)) return {};
  return Convert(bounds);
}

std::unique_ptr<IGeometry> QuartzGeometry::Transform(const Matrix &matrix) {
  auto cg_matrix = Convert(matrix);
  auto cg_path = CGPathCreateCopyByTransformingPath(cg_path_, &cg_matrix);
  return std::make_unique<QuartzGeometry>(GetGraphicsFactory(), cg_path);
}

std::unique_ptr<IGeometry> QuartzGeometry::CreateStrokeGeometry(float width) {
  auto cg_path = CGPathCreateCopyByStrokingPath(
      cg_path_, nullptr, width, kCGLineCapButt, kCGLineJoinMiter, 10);
  return std::make_unique<QuartzGeometry>(GetGraphicsFactory(), cg_path);
}

QuartzGeometryBuilder::QuartzGeometryBuilder(IGraphicsFactory *graphics_factory)
    : OsxQuartzResource(graphics_factory) {
  cg_mutable_path_ = CGPathCreateMutable();
}

QuartzGeometryBuilder::~QuartzGeometryBuilder() {
  CGPathRelease(cg_mutable_path_);
}

Point QuartzGeometryBuilder::GetCurrentPosition() {
  return Convert(CGPathGetCurrentPoint(cg_mutable_path_));
}

void QuartzGeometryBuilder::MoveTo(const Point &point) {
  CGPathMoveToPoint(cg_mutable_path_, nullptr, point.x, point.y);
}

void QuartzGeometryBuilder::LineTo(const Point &point) {
  CGPathAddLineToPoint(cg_mutable_path_, nullptr, point.x, point.y);
}

void QuartzGeometryBuilder::CubicBezierTo(const Point &start_control_point,
                                          const Point &end_control_point,
                                          const Point &end_point) {
  CGPathAddCurveToPoint(cg_mutable_path_, nullptr, start_control_point.x,
                        start_control_point.y, end_control_point.x,
                        end_control_point.y, end_point.x, end_point.y);
}

void QuartzGeometryBuilder::QuadraticBezierTo(const Point &control_point,
                                              const Point &end_point) {
  CGPathAddQuadCurveToPoint(cg_mutable_path_, nullptr, control_point.x,
                            control_point.y, end_point.x, end_point.y);
}

void QuartzGeometryBuilder::CloseFigure(bool close) {
  if (close) CGPathCloseSubpath(cg_mutable_path_);
}

std::unique_ptr<IGeometry> QuartzGeometryBuilder::Build() {
  return std::make_unique<QuartzGeometry>(GetGraphicsFactory(),
                                          CGPathCreateCopy(cg_mutable_path_));
}
}  // namespace cru::platform::graphics::quartz
