#pragma once
#include "Resource.hpp"
#include "cru/platform/graphics/Geometry.hpp"

#include <memory>

#include <CoreGraphics/CoreGraphics.h>

namespace cru::platform::graphics::osx::quartz {
class QuartzGeometry : public OsxQuartzResource, public virtual IGeometry {
 public:
  QuartzGeometry(IGraphFactory *graphics_factory, CGContextRef cg_context,
                 CGPathRef cg_path);

  CRU_DELETE_COPY(QuartzGeometry)
  CRU_DELETE_MOVE(QuartzGeometry)

  ~QuartzGeometry() override;

  CGPathRef GetCGPath() const { return cg_path_; }

  bool FillContains(const Point &point) override;

 private:
  CGContextRef cg_context_;
  CGPathRef cg_path_;
};

namespace details {
struct GeometryBeginFigureAction : Object {
  explicit GeometryBeginFigureAction(Point point) : point(point) {}

  Point point;
};

struct GeometryCloseFigureAction : Object {
  explicit GeometryCloseFigureAction(bool close) : close(close) {}

  bool close;
};

struct GeometryLineToAction : Object {
  explicit GeometryLineToAction(Point point) : point(point) {}

  Point point;
};

struct GeometryQuadraticBezierToAction : Object {
  GeometryQuadraticBezierToAction(Point control_point, Point end_point)
      : control_point(control_point), end_point(end_point) {}

  Point control_point;
  Point end_point;
};
}  // namespace details

class QuartzGeometryBuilder : public OsxQuartzResource,
                              public virtual IGeometryBuilder {
 public:
  explicit QuartzGeometryBuilder(IGraphFactory *graphics_factory,
                                 CGContextRef cg_context);

  CRU_DELETE_COPY(QuartzGeometryBuilder)
  CRU_DELETE_MOVE(QuartzGeometryBuilder)

  ~QuartzGeometryBuilder() override = default;

  void BeginFigure(const Point &point) override {
    actions_.push_back(
        std::make_unique<details::GeometryBeginFigureAction>(point));
  }
  void CloseFigure(bool close) override {
    actions_.push_back(
        std::make_unique<details::GeometryCloseFigureAction>(close));
  }
  void LineTo(const Point &point) override {
    actions_.push_back(std::make_unique<details::GeometryLineToAction>(point));
  }
  void QuadraticBezierTo(const Point &control_point,
                         const Point &end_point) override {
    actions_.push_back(
        std::make_unique<details::GeometryQuadraticBezierToAction>(
            control_point, end_point));
  }

  std::unique_ptr<IGeometry> Build() override;

 private:
  CGContextRef cg_context_;

  std::vector<std::unique_ptr<Object>> actions_;
};
}  // namespace cru::platform::graphics::osx::quartz
