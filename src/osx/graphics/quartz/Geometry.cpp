#include "cru/osx/graphics/quartz/Geometry.hpp"

#include <memory>

namespace cru::platform::graphics::osx::quartz {
QuartzGeometry::QuartzGeometry(IGraphFactory* graphics_factory,
                               CGContextRef cg_context, CGPathRef cg_path)
    : OsxQuartzResource(graphics_factory),
      cg_context_(cg_context),
      cg_path_(cg_path) {}

QuartzGeometry::~QuartzGeometry() { CGPathRelease(cg_path_); }

bool QuartzGeometry::FillContains(const Point& point) {
  CGContextBeginPath(cg_context_);
  CGContextAddPath(cg_context_, cg_path_);
  return CGContextPathContainsPoint(cg_context_, CGPoint{point.x, point.y},
                                    kCGPathFill);
}

QuartzGeometryBuilder::QuartzGeometryBuilder(IGraphFactory* graphics_factory,
                                             CGContextRef cg_context)
    : OsxQuartzResource(graphics_factory), cg_context_(cg_context) {}

std::unique_ptr<IGeometry> QuartzGeometryBuilder::Build() {
  CGContextBeginPath(cg_context_);

  for (const auto& action : actions_) {
    Object* o = action.get();

    if (auto a = dynamic_cast<details::GeometryBeginFigureAction*>(o)) {
      CGContextMoveToPoint(cg_context_, a->point.x, a->point.y);
    } else if (auto a = dynamic_cast<details::GeometryCloseFigureAction*>(o)) {
      if (a->close) CGContextClosePath(cg_context_);
    } else if (auto a = dynamic_cast<details::GeometryLineToAction*>(o)) {
      CGContextAddLineToPoint(cg_context_, a->point.x, a->point.y);
    } else if (auto a =
                   dynamic_cast<details::GeometryQuadraticBezierToAction*>(o)) {
      CGContextAddQuadCurveToPoint(cg_context_, a->control_point.x,
                                   a->control_point.y, a->end_point.x,
                                   a->end_point.y);
    }
  }

  return std::make_unique<QuartzGeometry>(GetGraphFactory(), cg_context_,
                                          CGContextCopyPath(cg_context_));
}
}  // namespace cru::platform::graphics::osx::quartz
