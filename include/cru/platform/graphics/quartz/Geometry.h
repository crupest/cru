#pragma once
#include "Resource.h"
#include "cru/platform/graphics/Geometry.h"

#include <memory>

#include <CoreGraphics/CoreGraphics.h>

namespace cru::platform::graphics::quartz {
class QuartzGeometry : public OsxQuartzResource, public virtual IGeometry {
 public:
  QuartzGeometry(IGraphicsFactory *graphics_factory, CGPathRef cg_path);

  CRU_DELETE_COPY(QuartzGeometry)
  CRU_DELETE_MOVE(QuartzGeometry)

  ~QuartzGeometry() override;

  CGPathRef GetCGPath() const { return cg_path_; }

  bool FillContains(const Point &point) override;
  Rect GetBounds() override;

  std::unique_ptr<IGeometry> Transform(const Matrix &matrix) override;
  std::unique_ptr<IGeometry> CreateStrokeGeometry(float width) override;

 private:
  CGPathRef cg_path_;
};

class QuartzGeometryBuilder : public OsxQuartzResource,
                              public virtual IGeometryBuilder {
 public:
  explicit QuartzGeometryBuilder(IGraphicsFactory *graphics_factory);

  CRU_DELETE_COPY(QuartzGeometryBuilder)
  CRU_DELETE_MOVE(QuartzGeometryBuilder)

  ~QuartzGeometryBuilder() override;

  Point GetCurrentPosition() override;

  void MoveTo(const Point &point) override;
  void LineTo(const Point &point) override;
  void CubicBezierTo(const Point &start_control_point,
                     const Point &end_control_point,
                     const Point &end_point) override;
  void QuadraticBezierTo(const Point &control_point,
                         const Point &end_point) override;
  void CloseFigure(bool close) override;

  std::unique_ptr<IGeometry> Build() override;

 private:
  CGMutablePathRef cg_mutable_path_;
};
}  // namespace cru::platform::graphics::quartz
