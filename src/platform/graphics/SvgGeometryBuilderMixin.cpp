#include "cru/platform/graphics/SvgGeometryBuilderMixin.h"
#include "cru/platform/Exception.h"

namespace cru::platform::graphics {
SvgGeometryBuilderMixin::SvgGeometryBuilderMixin() {}

SvgGeometryBuilderMixin::~SvgGeometryBuilderMixin() {}

Point SvgGeometryBuilderMixin::GetCurrentPosition() {
  throw PlatformUnsupportedException(
      GetPlatformId(), u"GetCurrentPosition",
      u"Svg-based geometry does not support get current position.");
}

void SvgGeometryBuilderMixin::MoveTo(const Point& point) {
  AppendCommand(u"M");
  Append(point);
}

void SvgGeometryBuilderMixin::RelativeMoveTo(const Point& offset) {
  AppendCommand(u"m");
  Append(offset);
}

void SvgGeometryBuilderMixin::LineTo(const Point& point) {
  AppendCommand(u"L");
  Append(point);
}

void SvgGeometryBuilderMixin::RelativeLineTo(const Point& offset) {
  AppendCommand(u"l");
  Append(offset);
}

void SvgGeometryBuilderMixin::CubicBezierTo(const Point& start_control_point,
                                            const Point& end_control_point,
                                            const Point& end_point) {
  AppendCommand(u"C");
  Append(start_control_point);
  Append(end_control_point);
  Append(end_point);
}

void SvgGeometryBuilderMixin::RelativeCubicBezierTo(
    const Point& start_control_offset, const Point& end_control_offset,
    const Point& end_offset) {
  AppendCommand(u"c");
  Append(start_control_offset);
  Append(end_control_offset);
  Append(end_offset);
}

void SvgGeometryBuilderMixin::QuadraticBezierTo(const Point& control_point,
                                                const Point& end_point) {
  AppendCommand(u"Q");
  Append(control_point);
  Append(end_point);
}

void SvgGeometryBuilderMixin::RelativeQuadraticBezierTo(
    const Point& control_offset, const Point& end_offset) {
  AppendCommand(u"q");
  Append(control_offset);
  Append(end_offset);
}

void SvgGeometryBuilderMixin::ArcTo(const Point& radius, float angle,
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

void SvgGeometryBuilderMixin::RelativeArcTo(const Point& radius, float angle,
                                            bool is_large_arc,
                                            bool is_clockwise,
                                            const Point& end_offset) {
  AppendCommand(u"a");
  Append(radius.x);
  Append(radius.y);
  Append(angle);
  Append(is_large_arc);
  Append(is_clockwise);
  Append(end_offset);
}

void SvgGeometryBuilderMixin::CloseFigure(bool close) {
  if (close) AppendCommand(u"z");
}

void SvgGeometryBuilderMixin::ParseAndApplySvgPathData(StringView path_d) {
  AppendCommand(path_d);
}

void SvgGeometryBuilderMixin::AppendCommand(StringView command) {
  current_ += command;
  current_ += u' ';
}

void SvgGeometryBuilderMixin::Append(bool flag) {
  current_ += flag ? u'1' : u'0';
  current_ += u' ';
}

void SvgGeometryBuilderMixin::Append(float number) {
  current_ += ToString(number);
  current_ += u' ';
}

void SvgGeometryBuilderMixin::Append(const Point& point) {
  current_ += ToString(point.x);
  current_ += u',';
  current_ += ToString(point.y);
  current_ += u' ';
}
}  // namespace cru::platform::graphics
