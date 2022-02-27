#pragma once
#include "Resource.h"

namespace cru::platform::graphics {
struct CRU_PLATFORM_GRAPHICS_API IGeometry : virtual IGraphicsResource {
  virtual bool FillContains(const Point& point) = 0;
};

// After called Build, calling every method will throw a

struct CRU_PLATFORM_GRAPHICS_API IGeometryBuilder : virtual IGraphicsResource {
  virtual Point GetCurrentPosition() = 0;

  virtual void MoveTo(const Point& point) = 0;
  void RelativeMoveTo(const Point& offset) {
    MoveTo(GetCurrentPosition() + offset);
  }

  virtual void LineTo(const Point& point) = 0;
  void RelativeLineTo(const Point& offset) {
    LineTo(GetCurrentPosition() + offset);
  }

  virtual void CubicBezierTo(const Point& start_control_point,
                             const Point& end_control_point,
                             const Point& end_point) = 0;
  void RelativeCubicBezierTo(const Point& start_control_offset,
                             const Point& end_control_offset,
                             const Point& end_offset) {
    auto current_position = GetCurrentPosition();
    CubicBezierTo(current_position + start_control_offset,
                  current_position + end_control_offset,
                  current_position + end_offset);
  }

  virtual void QuadraticBezierTo(const Point& control_point,
                                 const Point& end_point) = 0;
  void RelativeQuadraticBezierTo(const Point& control_offset,
                                 const Point& end_offset) {
    auto current_position = GetCurrentPosition();
    QuadraticBezierTo(current_position + control_offset,
                      current_position + end_offset);
  }

  virtual void ArcTo(const Point& radius, float angle, bool is_large_arc,
                     bool is_clockwise, const Point& end_point) = 0;
  void RelativeArcTo(const Point& radius, float angle, bool is_large_arc,
                     bool is_clockwise, const Point& end_offset) {
    ArcTo(radius, angle, is_large_arc, is_clockwise,
          GetCurrentPosition() + end_offset);
  }

  virtual void CloseFigure(bool close) = 0;

  virtual std::unique_ptr<IGeometry> Build() = 0;
};
}  // namespace cru::platform::graphics
