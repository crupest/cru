#pragma once

#include "Geometry.h"

#include <format>
#include <utility>

namespace cru::platform::graphics {
/**
 * \remarks This class is a helper mixin for some platforms, especially web
 * canvas. It constructs a svg data by recording commands. Use GetPathData to
 * get svg path data and use it to build real geometry.
 */
class CRU_PLATFORM_GRAPHICS_API SvgGeometryBuilderMixin
    : public virtual IGeometryBuilder {
 public:
  SvgGeometryBuilderMixin();

  ~SvgGeometryBuilderMixin() override;

  Point GetCurrentPosition() override;

  void MoveTo(const Point& point) override;
  void RelativeMoveTo(const Point& offset) override;

  void LineTo(const Point& point) override;
  void RelativeLineTo(const Point& offset) override;

  void CubicBezierTo(const Point& start_control_point,
                     const Point& end_control_point,
                     const Point& end_point) override;
  void RelativeCubicBezierTo(const Point& start_control_offset,
                             const Point& end_control_offset,
                             const Point& end_offset) override;

  void QuadraticBezierTo(const Point& control_point,
                         const Point& end_point) override;
  void RelativeQuadraticBezierTo(const Point& control_offset,
                                 const Point& end_offset) override;

  void ArcTo(const Point& radius, float angle, bool is_large_arc,
             bool is_clockwise, const Point& end_point) override;
  void RelativeArcTo(const Point& radius, float angle, bool is_large_arc,
                     bool is_clockwise, const Point& end_offset) override;

  void CloseFigure(bool close) override;

  void ParseAndApplySvgPathData(StringView path_d) override;

 protected:
  String GetPathData() const { return current_; }

 private:
  template <typename... Args>
  void Append(StringView format, Args&&... args) {
    current_ += String::FromUtf8(
        std::format(format.ToUtf8(), std::forward<Args>(args)...));
    current_ += u' ';
  }

  void AppendCommand(StringView command);
  void Append(bool flag);
  void Append(float number);
  void Append(const Point& point);

 private:
  String current_;
};
}  // namespace cru::platform::graphics
