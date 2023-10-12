#include "cru/platform/graphics/SvgGeometry.h"
#include "cru/platform/Exception.h"
#include "cru/platform/graphics/Geometry.h"

namespace cru::platform::graphics {
SvgGeometryBuilder::SvgGeometryBuilder() {}

SvgGeometryBuilder::~SvgGeometryBuilder() {}

Point SvgGeometryBuilder::GetCurrentPosition() {
  throw PlatformUnsupportedException(
      GetPlatformId(), u"GetCurrentPosition",
      u"Svg geometry does not support get current position.");
}

void SvgGeometryBuilder::MoveTo(const Point& point) {
  AppendCommand(u"M");
  Append(point);
}

void SvgGeometryBuilder::RelativeMoveTo(const Point& offset) {
  AppendCommand(u"m");
  Append(offset);
}

void SvgGeometryBuilder::LineTo(const Point& point) {
  AppendCommand(u"L");
  Append(point);
}

void SvgGeometryBuilder::RelativeLineTo(const Point& offset) {
  AppendCommand(u"l");
  Append(offset);
}

void SvgGeometryBuilder::CubicBezierTo(const Point& start_control_point,
                                       const Point& end_control_point,
                                       const Point& end_point) {
  AppendCommand(u"C");
  Append(start_control_point);
  Append(end_control_point);
  Append(end_point);
}

void SvgGeometryBuilder::RelativeCubicBezierTo(
    const Point& start_control_offset, const Point& end_control_offset,
    const Point& end_offset) {
  AppendCommand(u"c");
  Append(start_control_offset);
  Append(end_control_offset);
  Append(end_offset);
}

void SvgGeometryBuilder::QuadraticBezierTo(const Point& control_point,
                                           const Point& end_point) {
  AppendCommand(u"Q");
  Append(control_point);
  Append(end_point);
}

void SvgGeometryBuilder::RelativeQuadraticBezierTo(const Point& control_offset,
                                                   const Point& end_offset) {
  AppendCommand(u"q");
  Append(control_offset);
  Append(end_offset);
}

void SvgGeometryBuilder::ArcTo(const Point& radius, float angle,
                               bool is_large_arc, bool is_clockwise,
                               const Point& end_point) {
  AppendCommand(u"A");
  Append(radius.x);
  Append(radius.y);
  Append(angle);
  Append(is_large_arc);
  Append(is_clockwise);
  Append(end_point);
}

void SvgGeometryBuilder::RelativeArcTo(const Point& radius, float angle,
                                       bool is_large_arc, bool is_clockwise,
                                       const Point& end_offset) {
  AppendCommand(u"a");
  Append(radius.x);
  Append(radius.y);
  Append(angle);
  Append(is_large_arc);
  Append(is_clockwise);
  Append(end_offset);
}

void SvgGeometryBuilder::CloseFigure(bool close) {
  if (close) AppendCommand(u"z");
}

std::unique_ptr<IGeometry> SvgGeometryBuilder::Build() {
  throw PlatformUnsupportedException(
      u"SvgGeometryBuilder", u"Build",
      u"SvgGeometryBuilder can't build a Geometry. Use GetPathData instead.");
}

void SvgGeometryBuilder::ParseAndApplySvgPathData(StringView path_d) {
  AppendCommand(path_d);
}

void SvgGeometryBuilder::AppendCommand(StringView command) {
  current_ += command;
  current_ += u' ';
}

void SvgGeometryBuilder::Append(bool flag) {
  current_ += flag ? u'1' : u'0';
  current_ += u' ';
}

void SvgGeometryBuilder::Append(float number) {
  current_ += ToString(number);
  current_ += u' ';
}

void SvgGeometryBuilder::Append(const Point& point) {
  current_ += ToString(point.x);
  current_ += u',';
  current_ += ToString(point.y);
  current_ += u' ';
}
}  // namespace cru::platform::graphics
