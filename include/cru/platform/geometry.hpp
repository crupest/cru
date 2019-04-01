#pragma once
#include "cru/common/base.hpp"

#include "basic_types.hpp"
#include "cru/common/ui_base.hpp"

namespace cru::platform {
struct Geometry : virtual Interface {
  virtual bool FillContains(const ui::Point& point) = 0;
};

struct GeometryBuilder : virtual Interface {
  virtual bool IsValid() = 0;
  virtual void BeginFigure(const ui::Point& point) = 0;
  virtual void LineTo(const ui::Point& point) = 0;
  virtual void QuadraticBezierTo(const ui::Point& control_point,
                                 const ui::Point& end_point) = 0;
  virtual void CloseFigure(bool close) = 0;
  virtual Geometry* Build() = 0;

  Geometry* BuildAndDeleteThis() {
    Build();
    delete this;
  }
};
}  // namespace cru::platform
