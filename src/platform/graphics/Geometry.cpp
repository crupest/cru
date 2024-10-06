#include "cru/platform/graphics/Geometry.h"

#include "cru/base/Exception.h"
#include "cru/platform/Exception.h"
#include "cru/platform/graphics/Factory.h"

#include <cmath>
#include <unordered_set>

namespace cru::platform::graphics {
bool IGeometry::StrokeContains(float width, const Point& point) {
  auto geometry = CreateStrokeGeometry(width);
  return geometry->FillContains(point);
}

std::unique_ptr<IGeometry> IGeometry::CreateStrokeGeometry(
    [[maybe_unused]] float width) {
  throw PlatformUnsupportedException(GetPlatformId(), u"CreateStrokeGeometry",
                                     u"Create stroke geometry of a geometry is "
                                     u"not supported on this platform.");
}

void IGeometryBuilder::RelativeMoveTo(const Point& offset) {
  MoveTo(GetCurrentPosition() + offset);
}

void IGeometryBuilder::RelativeLineTo(const Point& offset) {
  LineTo(GetCurrentPosition() + offset);
}

void IGeometryBuilder::RelativeCubicBezierTo(const Point& start_control_offset,
                                             const Point& end_control_offset,
                                             const Point& end_offset) {
  auto current_position = GetCurrentPosition();
  CubicBezierTo(current_position + start_control_offset,
                current_position + end_control_offset,
                current_position + end_offset);
}

void IGeometryBuilder::RelativeQuadraticBezierTo(const Point& control_offset,
                                                 const Point& end_offset) {
  auto current_position = GetCurrentPosition();
  QuadraticBezierTo(current_position + control_offset,
                    current_position + end_offset);
}

void IGeometryBuilder::RelativeArcTo(const Point& radius, float angle,
                                     bool is_large_arc, bool is_clockwise,
                                     const Point& end_offset) {
  ArcTo(radius, angle, is_large_arc, is_clockwise,
        GetCurrentPosition() + end_offset);
}

constexpr float PI = 3.14159265358979323846f;

using std::abs;
using std::atan2;
using std::ceil;
using std::cos;
using std::sin;
using std::sqrt;

// The following codes are from Qt.

static void pathArcSegment(IGeometryBuilder* path, float xc, float yc,
                           float th0, float th1, float rx, float ry,
                           float xAxisRotation) {
  float sinTh, cosTh;
  float a00, a01, a10, a11;
  float x1, y1, x2, y2, x3, y3;
  float t;
  float thHalf;

  sinTh = sin(xAxisRotation * (PI / 180.0));
  cosTh = cos(xAxisRotation * (PI / 180.0));

  a00 = cosTh * rx;
  a01 = -sinTh * ry;
  a10 = sinTh * rx;
  a11 = cosTh * ry;

  thHalf = 0.5 * (th1 - th0);
  t = (8.0 / 3.0) * sin(thHalf * 0.5) * sin(thHalf * 0.5) / sin(thHalf);
  x1 = xc + cos(th0) - t * sin(th0);
  y1 = yc + sin(th0) + t * cos(th0);
  x3 = xc + cos(th1);
  y3 = yc + sin(th1);
  x2 = x3 + t * sin(th1);
  y2 = y3 - t * cos(th1);

  path->CubicBezierTo({a00 * x1 + a01 * y1, a10 * x1 + a11 * y1},
                      {a00 * x2 + a01 * y2, a10 * x2 + a11 * y2},
                      {a00 * x3 + a01 * y3, a10 * x3 + a11 * y3});
}

// the arc handling code underneath is from XSVG (BSD license)
/*
 * Copyright  2002 USC/Information Sciences Institute
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Information Sciences Institute not be used in advertising or
 * publicity pertaining to distribution of the software without
 * specific, written prior permission.  Information Sciences Institute
 * makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * INFORMATION SCIENCES INSTITUTE DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL INFORMATION SCIENCES
 * INSTITUTE BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
 * OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
static void pathArc(IGeometryBuilder* path, float rx, float ry,
                    float x_axis_rotation, int large_arc_flag, int sweep_flag,
                    float x, float y, float curx, float cury) {
  const float Pr1 = rx * rx;
  const float Pr2 = ry * ry;

  if (!Pr1 || !Pr2) return;

  float sin_th, cos_th;
  float a00, a01, a10, a11;
  float x0, y0, x1, y1, xc, yc;
  float d, sfactor, sfactor_sq;
  float th0, th1, th_arc;
  int i, n_segs;
  float dx, dy, dx1, dy1, Px, Py, check;

  rx = abs(rx);
  ry = abs(ry);

  sin_th = sin(x_axis_rotation * (PI / 180.0));
  cos_th = cos(x_axis_rotation * (PI / 180.0));

  dx = (curx - x) / 2.0;
  dy = (cury - y) / 2.0;
  dx1 = cos_th * dx + sin_th * dy;
  dy1 = -sin_th * dx + cos_th * dy;
  Px = dx1 * dx1;
  Py = dy1 * dy1;
  /* Spec : check if radii are large enough */
  check = Px / Pr1 + Py / Pr2;
  if (check > 1) {
    rx = rx * sqrt(check);
    ry = ry * sqrt(check);
  }

  a00 = cos_th / rx;
  a01 = sin_th / rx;
  a10 = -sin_th / ry;
  a11 = cos_th / ry;
  x0 = a00 * curx + a01 * cury;
  y0 = a10 * curx + a11 * cury;
  x1 = a00 * x + a01 * y;
  y1 = a10 * x + a11 * y;
  /* (x0, y0) is current point in transformed coordinate space.
     (x1, y1) is new point in transformed coordinate space.
     The arc fits a unit-radius circle in this space.
  */
  d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
  if (!d) return;
  sfactor_sq = 1.0 / d - 0.25;
  if (sfactor_sq < 0) sfactor_sq = 0;
  sfactor = sqrt(sfactor_sq);
  if (sweep_flag == large_arc_flag) sfactor = -sfactor;
  xc = 0.5 * (x0 + x1) - sfactor * (y1 - y0);
  yc = 0.5 * (y0 + y1) + sfactor * (x1 - x0);
  /* (xc, yc) is center of the circle. */

  th0 = atan2(y0 - yc, x0 - xc);
  th1 = atan2(y1 - yc, x1 - xc);

  th_arc = th1 - th0;
  if (th_arc < 0 && sweep_flag)
    th_arc += 2 * PI;
  else if (th_arc > 0 && !sweep_flag)
    th_arc -= 2 * PI;

  n_segs = ceil(abs(th_arc / (PI * 0.5 + 0.001)));

  for (i = 0; i < n_segs; i++) {
    pathArcSegment(path, xc, yc, th0 + i * th_arc / n_segs,
                   th0 + (i + 1) * th_arc / n_segs, rx, ry, x_axis_rotation);
  }
}

void IGeometryBuilder::ArcTo(const Point& radius, float angle,
                             bool is_large_arc, bool is_clockwise,
                             const Point& end_point) {
  auto current_position = GetCurrentPosition();
  pathArc(this, radius.x, radius.y, angle, is_large_arc, is_clockwise,
          end_point.x, end_point.y, current_position.x, current_position.y);
}

namespace {
const std::unordered_set<char16_t> kSvgPathDataCommands = {
    'M', 'm', 'L', 'l', 'H', 'h', 'V', 'v', 'C', 'c',
    'S', 's', 'Q', 'q', 'T', 't', 'A', 'a', 'Z', 'z'};
}

void IGeometryBuilder::ParseAndApplySvgPathData(StringView path_d) {
  Index position = 0;
  const auto size = path_d.size();

  char16_t last_command = 0;
  bool last_is_cubic = false;
  bool last_is_quad = false;
  Point last_end_point;
  Point last_control_point;

  auto read_spaces = [&] {
    while (position < size &&
           (path_d[position] == ' ' || path_d[position] == '\n')) {
      ++position;
    }
    return position == size;
  };

  auto read_number = [&] {
    if (read_spaces()) {
      throw Exception(u"Unexpected eof of svg path data command.");
    }

    if (path_d[position] == ',') {
      ++position;
    }

    Index processed_count = 0;

    auto result = path_d.substr(position).ParseToFloat(
        &processed_count, StringToNumberFlags::kAllowTrailingJunk);

    if (std::isnan(result)) throw Exception(u"Invalid svg path data number.");

    position += processed_count;

    return result;
  };

  auto read_point = [&] {
    auto x = read_number();
    auto y = read_number();
    return Point(x, y);
  };

  auto do_command = [&, this](char16_t command) {
    last_command = command;
    last_is_cubic = false;
    last_is_quad = false;

    switch (command) {
      case 'M':
        MoveTo(read_point());
        break;
      case 'm':
        RelativeMoveTo(read_point());
        break;
      case 'L':
        LineTo(read_point());
        break;
      case 'l':
        RelativeLineTo(read_point());
        break;
      case 'H':
        LineTo(read_number(), this->GetCurrentPosition().y);
        break;
      case 'h':
        RelativeLineTo(read_number(), 0);
        break;
      case 'V':
        LineTo(GetCurrentPosition().x, read_number());
        break;
      case 'v':
        RelativeLineTo(0, read_number());
        break;
      case 'C': {
        auto start_control_point = read_point(),
             end_control_point = read_point(), end_point = read_point();
        CubicBezierTo(start_control_point, end_control_point, end_point);
        last_is_cubic = true;
        last_control_point = end_control_point;
        last_end_point = end_point;
        break;
      }
      case 'c': {
        auto current_position = GetCurrentPosition();
        auto start_control_point = current_position + read_point(),
             end_control_point = current_position + read_point(),
             end_point = current_position + read_point();
        CubicBezierTo(start_control_point, end_control_point, end_point);
        last_is_cubic = true;
        last_control_point = end_control_point;
        last_end_point = end_point;
        break;
      }
      case 'S': {
        auto current_position = GetCurrentPosition();
        auto start_control_point = last_is_cubic ? Point{last_end_point.x * 2,
                                                         last_end_point.y * 2} -
                                                       last_control_point
                                                 : current_position,
             end_control_point = read_point(), end_point = read_point();
        CubicBezierTo(start_control_point, end_control_point, end_point);
        last_is_cubic = true;
        last_control_point = end_control_point;
        last_end_point = end_point;
        break;
      }
      case 's': {
        auto current_position = GetCurrentPosition();
        auto start_control_point = last_is_cubic ? Point{last_end_point.x * 2,
                                                         last_end_point.y * 2} -
                                                       last_control_point
                                                 : current_position,
             end_control_point = current_position + read_point(),
             end_point = current_position + read_point();
        CubicBezierTo(start_control_point, end_control_point, end_point);
        last_is_cubic = true;
        last_control_point = end_control_point;
        last_end_point = end_point;
        break;
      }
      case 'Q': {
        auto control_point = read_point(), end_point = read_point();
        QuadraticBezierTo(control_point, end_point);
        last_is_quad = true;
        last_control_point = control_point;
        last_end_point = end_point;
        break;
      }
      case 'q': {
        auto current_position = GetCurrentPosition();
        auto control_point = current_position + read_point(),
             end_point = current_position + read_point();
        QuadraticBezierTo(control_point, end_point);
        last_is_quad = true;
        last_control_point = control_point;
        last_end_point = end_point;
        break;
      }
      case 'T': {
        auto current_position = GetCurrentPosition();
        auto control_point = last_is_quad ? Point{last_end_point.x * 2,
                                                  last_end_point.y * 2} -
                                                last_control_point
                                          : current_position,
             end_point = read_point();
        QuadraticBezierTo(control_point, end_point);
        last_is_quad = true;
        last_control_point = control_point;
        last_end_point = end_point;
        break;
      }
      case 't': {
        auto current_position = GetCurrentPosition();
        auto control_point = last_is_quad ? Point{last_end_point.x * 2,
                                                  last_end_point.y * 2} -
                                                last_control_point
                                          : current_position,
             end_point = current_position + read_point();
        QuadraticBezierTo(control_point, end_point);
        last_is_quad = true;
        last_control_point = control_point;
        last_end_point = end_point;
        break;
      }
      case 'A':
        ArcTo({read_number(), read_number()}, read_number(), read_number(),
              read_number(), read_point());
        break;
      case 'a':
        RelativeArcTo({read_number(), read_number()}, read_number(),
                      read_number(), read_number(), read_point());
        break;
      case 'Z':
      case 'z':
        CloseFigure(true);
        break;
      default:
        throw Exception(u"Invalid svg path command.");
    }
    return true;
  };

  auto read_command = [&] {
    if (read_spaces()) {
      return false;
    }
    auto command = path_d[position];

    if (kSvgPathDataCommands.contains(command)) {
      position++;
      do_command(command);
    } else {
      do_command(last_command);
    }

    return true;
  };

  while (true) {
    if (!read_command()) break;
  }
}

std::unique_ptr<IGeometry> CreateGeometryFromSvgPathData(
    IGraphicsFactory* factory, StringView path_d) {
  auto builder = factory->CreateGeometryBuilder();
  builder->ParseAndApplySvgPathData(path_d);
  return builder->Build();
}

}  // namespace cru::platform::graphics
