#pragma once
#include "cru/common/base.hpp"

#include "cru/common/endable.hpp"
#include "cru/common/ui_base.hpp"

namespace cru::platform::graph {
struct IGeometry : virtual Interface {
  virtual bool FillContains(const ui::Point& point) = 0;
};

struct IGeometryBuilder : virtual Interface, virtual IEndable<IGeometry*> {
  virtual void BeginFigure(const ui::Point& point) = 0;
  virtual void LineTo(const ui::Point& point) = 0;
  virtual void QuadraticBezierTo(const ui::Point& control_point,
                                 const ui::Point& end_point) = 0;
  virtual void CloseFigure(bool close) = 0;
};
}  // namespace cru::platform::graph
