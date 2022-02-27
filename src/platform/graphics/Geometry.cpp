#include "cru/platform/graphics/Geometry.h"

#include <cmath>

namespace cru::platform::graphics {
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

}  // namespace cru::platform::graphics
