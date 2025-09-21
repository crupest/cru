#include "cru/platform/graphics/quartz/Geometry.h"

#include <cstdlib>
#include <memory>
#include <numbers>

namespace cru::platform::graphics::quartz {
QuartzGeometry::QuartzGeometry(IGraphicsFactory* graphics_factory,
                               CGPathRef cg_path)
    : OsxQuartzResource(graphics_factory), cg_path_(cg_path) {}

QuartzGeometry::~QuartzGeometry() { CGPathRelease(cg_path_); }

bool QuartzGeometry::FillContains(const Point& point) {
  return CGPathContainsPoint(cg_path_, nullptr, CGPoint{point.x, point.y},
                             kCGPathFill);
}

Rect QuartzGeometry::GetBounds() {
  auto bounds = CGPathGetPathBoundingBox(cg_path_);
  if (CGRectIsNull(bounds)) return {};
  return Convert(bounds);
}

std::unique_ptr<IGeometry> QuartzGeometry::Transform(const Matrix& matrix) {
  auto cg_matrix = Convert(matrix);
  auto cg_path = CGPathCreateCopyByTransformingPath(cg_path_, &cg_matrix);
  return std::make_unique<QuartzGeometry>(GetGraphicsFactory(), cg_path);
}

std::unique_ptr<IGeometry> QuartzGeometry::CreateStrokeGeometry(float width) {
  auto cg_path = CGPathCreateCopyByStrokingPath(
      cg_path_, nullptr, width, kCGLineCapButt, kCGLineJoinMiter, 10);
  return std::make_unique<QuartzGeometry>(GetGraphicsFactory(), cg_path);
}

QuartzGeometryBuilder::QuartzGeometryBuilder(IGraphicsFactory* graphics_factory)
    : OsxQuartzResource(graphics_factory) {
  cg_mutable_path_ = CGPathCreateMutable();
}

QuartzGeometryBuilder::~QuartzGeometryBuilder() {
  CGPathRelease(cg_mutable_path_);
}

Point QuartzGeometryBuilder::GetCurrentPosition() {
  return Convert(CGPathGetCurrentPoint(cg_mutable_path_));
}

void QuartzGeometryBuilder::MoveTo(const Point& point) {
  CGPathMoveToPoint(cg_mutable_path_, nullptr, point.x, point.y);
}

void QuartzGeometryBuilder::LineTo(const Point& point) {
  CGPathAddLineToPoint(cg_mutable_path_, nullptr, point.x, point.y);
}

void QuartzGeometryBuilder::CubicBezierTo(const Point& start_control_point,
                                          const Point& end_control_point,
                                          const Point& end_point) {
  CGPathAddCurveToPoint(cg_mutable_path_, nullptr, start_control_point.x,
                        start_control_point.y, end_control_point.x,
                        end_control_point.y, end_point.x, end_point.y);
}

void QuartzGeometryBuilder::QuadraticBezierTo(const Point& control_point,
                                              const Point& end_point) {
  CGPathAddQuadCurveToPoint(cg_mutable_path_, nullptr, control_point.x,
                            control_point.y, end_point.x, end_point.y);
}

void QuartzGeometryBuilder::ArcTo(const Point& radius, float angle,
                                  bool is_large_arc, bool is_clockwise,
                                  const Point& end_point) {
  auto pos = GetCurrentPosition();
  auto info = CalculateArcInfo(pos, radius, angle, is_large_arc, is_clockwise,
                               end_point);

  // Compose the full transform: rotate first, then scale, then translate.
  // This matches Cairo's cairo_translate → cairo_scale → cairo_rotate
  // pattern (note: Cairo applies transforms in reverse order to CG).
  // The arc is drawn as a unit circle at origin and the transform turns it
  // into the correctly positioned, scaled, and rotated ellipse.
  auto matrix = Matrix::Rotation(angle) * Matrix::Scale(radius.x, radius.y) *
                Matrix::Translation(info.center.x, info.center.y);
  CGAffineTransform transform = Convert(matrix);

  // CGPathAddArc's clockwise parameter uses Quartz's y-down user space, but
  // the sweep direction is opposite to SVG's sweep-flag in that space. SVG
  // sweep-flag = 1 means counter-clockwise on screen (positive angle in
  // y-down), which is CGPathAddArc's clockwise=false direction; sweep-flag = 0
  // means the opposite. We also need to adjust the end angle so the signed
  // sweep matches the requested arc size, just like the fallback pathArc()
  // does.
  auto start_angle = info.start_angle;
  auto end_angle = info.end_angle;
  auto sweep = end_angle - start_angle;
  constexpr auto two_pi = 2.0f * std::numbers::pi_v<float>;
  if (sweep < 0 && is_clockwise) {
    sweep += two_pi;
  } else if (sweep > 0 && !is_clockwise) {
    sweep -= two_pi;
  }
  end_angle = start_angle + sweep;

  CGPathAddArc(cg_mutable_path_, &transform, 0, 0, 1, start_angle, end_angle,
               !is_clockwise);
}

void QuartzGeometryBuilder::CloseFigure(bool close) {
  if (close) CGPathCloseSubpath(cg_mutable_path_);
}

std::unique_ptr<IGeometry> QuartzGeometryBuilder::Build() {
  return std::make_unique<QuartzGeometry>(GetGraphicsFactory(),
                                          CGPathCreateCopy(cg_mutable_path_));
}
}  // namespace cru::platform::graphics::quartz
