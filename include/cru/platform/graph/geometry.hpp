#pragma once
#include "resource.hpp"

#include <memory>

namespace cru::platform::graph {
struct IGeometry : virtual IGraphResource {
  virtual bool FillContains(const Point& point) = 0;
};

// After called Build, calling every method will throw a

class IGeometryBuilder : virtual IGraphResource {
  virtual void BeginFigure(const Point& point) = 0;
  virtual void LineTo(const Point& point) = 0;
  virtual void QuadraticBezierTo(const Point& control_point,
                                 const Point& end_point) = 0;
  virtual void CloseFigure(bool close) = 0;

  virtual std::unique_ptr<IGeometry> Build() = 0;
};
}  // namespace cru::platform::graph
