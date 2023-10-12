#pragma once

#include "Geometry.h"
#include "cru/common/Base.h"
#include "cru/common/Format.h"

#include <utility>

namespace cru::platform::graphics {
/**
 * \remarks This class is purely a helper for some platforms, especially web
 * canvas. It constructs a path data of svg of a list of commands. It can't
 * generate a Geometry. Calling Build will throw a PlatformUnsupportedException.
 * Instead, use GetPathData to get svg path data and use it to do other things.
 */
class CRU_PLATFORM_GRAPHICS_API SvgGeometryBuilder
    : public Object,
      public virtual IGeometryBuilder {
 public:
  SvgGeometryBuilder();

  CRU_DELETE_COPY(SvgGeometryBuilder)
  CRU_DELETE_MOVE(SvgGeometryBuilder)

  ~SvgGeometryBuilder() override;

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

  std::unique_ptr<IGeometry> Build() override;

  String GetPathData() const { return current_; }

  void ParseAndApplySvgPathData(StringView path_d) override;

  template <typename... Args>
  void Append(StringView format, Args&&... args) {
    current_ += Format(format, std::forward<Args>(args)...);
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
