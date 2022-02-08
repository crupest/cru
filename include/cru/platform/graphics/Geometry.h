#pragma once
#include "Resource.h"

namespace cru::platform::graphics {
struct CRU_PLATFORM_GRAPHICS_API IGeometry : virtual IGraphicsResource {
  virtual bool FillContains(const Point& point) = 0;
};

// After called Build, calling every method will throw a

struct CRU_PLATFORM_GRAPHICS_API IGeometryBuilder : virtual IGraphicsResource {
  virtual void BeginFigure(const Point& point) = 0;
  virtual void LineTo(const Point& point) = 0;
  virtual void QuadraticBezierTo(const Point& control_point,
                                 const Point& end_point) = 0;
  virtual void CloseFigure(bool close) = 0;

  virtual std::unique_ptr<IGeometry> Build() = 0;
};
}  // namespace cru::platform::graphics
