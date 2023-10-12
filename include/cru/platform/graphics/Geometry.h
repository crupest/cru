#pragma once
#include "Resource.h"

#include <memory>

namespace cru::platform::graphics {
/**
 * \remarks Geometry implementation is a disaster zone of platform problems.
 * Here are some notes. For geometry object:
 * 1. Some platforms just lack the concept of geometry. Even if there is one, it
 * usually has limited capabilities.
 * 2. Doing hit test right on the geometry out of context is not supported on
 * some platform. The geometry needs to be put on the context first.
 * 3. Create a geometry of a stroke of another geometry is not suppored more
 * widely.
 *
 * Workarounds:
 * 1. If the platform didn't support path at all, although not a platform does
 * so far, record the commands and replay it on context when it is used.
 * 2. If the geometry can't do hit test itself, always attach a canvas with it,
 * and when do hit test, put it on context first.
 * 3. There is no good workaround for 4, so don't use it, maybe.
 *
 * For geometry builder:
 * It is hard to conclude the common commands of all platforms. Some have this,
 * others not. Some don't have this, but others do. There are some common
 * commands that should be used frequently. If some platforms do not support it,
 * do it yourself to implement it with existing commands and fancy algorithm.
 *
 */
struct CRU_PLATFORM_GRAPHICS_API IGeometry : virtual IGraphicsResource {
  /**
   * \remarks Because I designed CreateStrokeGeometry first, which might be not
   * that rational. So the default implementation calls that method and do the
   * test. New implementation should override this.
   */
  virtual bool StrokeContains(float width, const Point& point);
  virtual bool FillContains(const Point& point) = 0;
  virtual Rect GetBounds() = 0;

  /**
   * \remarks See class doc for platform limitation.
   */
  virtual std::unique_ptr<IGeometry> Transform(const Matrix& matrix) = 0;

  /**
   * \remarks This method is a little quirky. It is originated from Direct2D,
   * but many platforms do not support this, I guess. So don't use this anymore.
   */
  virtual std::unique_ptr<IGeometry> CreateStrokeGeometry(float width);
};

/**
 * \remarks We always run into platform porting problems. When I designed this
 * interface, I thought every platform would support GetCurrentPosition.
 * However, this is not actually true.
 * For web canvas, it turns out that it can't get current position. So I have to
 * not implement or use it, which disables every relative actions because they
 * depend on it. But luckily, the web canvas path can be constructed from svg
 * path (see https://developer.mozilla.org/en-US/docs/Web/API/Path2D/Path2D). So
 * I come up with a new idea that first create a geometry describe itself in
 * svg, and then construct path from it. And all relative methods should become
 * virtual so it can override them.
 */
struct CRU_PLATFORM_GRAPHICS_API IGeometryBuilder : virtual IGraphicsResource {
  virtual Point GetCurrentPosition() = 0;

  virtual void MoveTo(const Point& point) = 0;
  virtual void RelativeMoveTo(const Point& offset);

  virtual void LineTo(const Point& point) = 0;
  void LineTo(float x, float y) { LineTo(Point(x, y)); }
  virtual void RelativeLineTo(const Point& offset);
  void RelativeLineTo(float x, float y) { RelativeLineTo(Point(x, y)); }

  virtual void CubicBezierTo(const Point& start_control_point,
                             const Point& end_control_point,
                             const Point& end_point) = 0;
  virtual void RelativeCubicBezierTo(const Point& start_control_offset,
                                     const Point& end_control_offset,
                                     const Point& end_offset);

  virtual void QuadraticBezierTo(const Point& control_point,
                                 const Point& end_point) = 0;
  virtual void RelativeQuadraticBezierTo(const Point& control_offset,
                                         const Point& end_offset);

  virtual void ArcTo(const Point& radius, float angle, bool is_large_arc,
                     bool is_clockwise, const Point& end_point);
  virtual void RelativeArcTo(const Point& radius, float angle,
                             bool is_large_arc, bool is_clockwise,
                             const Point& end_offset);

  virtual void CloseFigure(bool close) = 0;

  virtual std::unique_ptr<IGeometry> Build() = 0;

  virtual void ParseAndApplySvgPathData(StringView path_d);
};

std::unique_ptr<IGeometry> CRU_PLATFORM_GRAPHICS_API
CreateGeometryFromSvgPathData(IGraphicsFactory* factory, StringView path_d);
}  // namespace cru::platform::graphics
