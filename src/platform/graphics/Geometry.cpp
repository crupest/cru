#include "cru/platform/graphics/Geometry.h"
#include "cru/base/StringUtil.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/GraphicsBase.h"
#include "cru/platform/Matrix.h"
#include "cru/platform/graphics/Factory.h"

#include <cmath>
#include <numbers>
#include <unordered_set>

namespace cru::platform::graphics {
bool IGeometry::StrokeContains(float width, const Point& point) {
  auto geometry = CreateStrokeGeometry(width);
  return geometry->FillContains(point);
}

std::unique_ptr<IGeometry> IGeometry::CreateStrokeGeometry(
    [[maybe_unused]] float width) {
  throw PlatformUnsupportedException(GetPlatformId(), "CreateStrokeGeometry",
                                     "Create stroke geometry of a geometry is "
                                     "not supported on this platform.");
}

IGeometryBuilder::ArcInfo IGeometryBuilder::CalculateArcInfo(
    const Point& start_point, const Point& radius, float angle,
    bool is_large_arc, bool is_clockwise, const Point& end_point) {
  if (radius.x == 0.0f || radius.y == 0.0f) {
    CruLogWarn(kLogTag, "Invalid Arc: radius is zero.");
    return {};
  }

  // Transform the ellipse to a unit circle: first rotate by -angle to align
  // the axes, then scale by the inverse radii. Matrix multiplication applies
  // right-to-left for our row-vector convention, so Scale comes first.
  auto matrix =
      Matrix::Scale(1 / radius.x, 1 / radius.y) * Matrix::Rotation(-angle);
  auto s1 = matrix.TransformPoint(start_point),
       s2 = matrix.TransformPoint(end_point);

  // Handle full ellipse (start == end after transform).
  if (s1 == s2) {
    Point center(s1.x - 1.0f, s1.y);
    float a1 = std::atan2(s1.y - center.y, s1.x - center.x);
    return {matrix.Inverted()->TransformPoint(center), radius.x, radius.y, a1,
            a1 + 2.0f * std::numbers::pi_v<float>};
  }

  auto mid = (s1 + s2) / 2;
  auto d = s1.Distance(s2) / 2;

  // Per SVG spec (F.6.6): if the distance between endpoints exceeds the
  // diameter, scale the radii proportionally so the arc becomes valid.
  if (d > 1.0f) {
    Point scaled_radius(radius.x * d, radius.y * d);
    return CalculateArcInfo(start_point, scaled_radius, angle, is_large_arc,
                            is_clockwise, end_point);
  }

  auto dc = std::sqrt(1 - d * d);
  // Perpendicular direction from chord midpoint to the center.
  auto a = std::atan2(s1.x - s2.x, s2.y - s1.y);

  // The two possible unit-circle centers are mid ± dc*perp.
  // Which one to pick depends on both the sweep direction and the arc size:
  //   clockwise small  → -dc (is_clockwise=T, is_large_arc=F: !=)
  //   clockwise large  → +dc (is_clockwise=T, is_large_arc=T: ==)
  //   ccw small        → +dc (is_clockwise=F, is_large_arc=F: ==)
  //   ccw large        → -dc (is_clockwise=F, is_large_arc=T: !=)
  float sign = (is_clockwise == is_large_arc) ? 1.0f : -1.0f;
  Point center(mid.x + sign * dc * std::cos(a),
               mid.y + sign * dc * std::sin(a));

  if (std::abs(center.x) < 0.000001) {
    center.x = 0.f;
  }
  if (std::abs(center.y) < 0.000001) {
    center.y = 0.f;
  }

  auto a1 = std::atan2(s1.y - center.y, s1.x - center.x);
  auto a2 = std::atan2(s2.y - center.y, s2.x - center.x);

  return {matrix.Inverted()->TransformPoint(center), radius.x, radius.y, a1,
          a2};
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

namespace {
const std::unordered_set<char16_t> kSvgPathDataCommands = {
    'M', 'm', 'L', 'l', 'H', 'h', 'V', 'v', 'C', 'c',
    'S', 's', 'Q', 'q', 'T', 't', 'A', 'a', 'Z', 'z'};
}

void IGeometryBuilder::ParseAndApplySvgPathData(std::string_view path_d) {
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
      throw Exception("Unexpected eof of svg path data command.");
    }

    if (path_d[position] == ',') {
      ++position;
    }

    auto result = cru::string::ParseToNumber<float>(
        path_d.substr(position),
        cru::string::ParseToNumberFlags::AllowTrailingJunk);

    if (!result.valid) throw Exception("Invalid svg path data number.");

    position += result.processed_char_count;

    return result.value;
  };

  auto read_point = [&] {
    auto x = read_number();
    auto y = read_number();
    return Point(x, y);
  };

  auto do_command = [&, this](char command) {
    last_command = command;
    last_is_cubic = false;
    last_is_quad = false;

    switch (command) {
      case 'M': {
        auto end = read_point();
        MoveTo(end);
        break;
      }
      case 'm': {
        auto offset = read_point();
        RelativeMoveTo(offset);
        break;
      }
      case 'L': {
        auto end = read_point();
        LineTo(end);
        break;
      }
      case 'l': {
        auto offset = read_point();
        RelativeLineTo(offset);
        break;
      }
      case 'H': {
        auto x = read_number();
        LineTo(x, this->GetCurrentPosition().y);
        break;
      }
      case 'h': {
        auto dx = read_number();
        RelativeLineTo(dx, 0);
        break;
      }
      case 'V': {
        auto y = read_number();
        LineTo(GetCurrentPosition().x, y);
        break;
      }
      case 'v': {
        auto dy = read_number();
        RelativeLineTo(0, dy);
        break;
      }
      case 'C': {
        auto start_control_point = read_point();
        auto end_control_point = read_point();
        auto end_point = read_point();
        CubicBezierTo(start_control_point, end_control_point, end_point);
        last_is_cubic = true;
        last_control_point = end_control_point;
        last_end_point = end_point;
        break;
      }
      case 'c': {
        auto current_position = GetCurrentPosition();
        auto start_control_point = current_position + read_point();
        auto end_control_point = current_position + read_point();
        auto end_point = current_position + read_point();
        CubicBezierTo(start_control_point, end_control_point, end_point);
        last_is_cubic = true;
        last_control_point = end_control_point;
        last_end_point = end_point;
        break;
      }
      case 'S': {
        auto current_position = GetCurrentPosition();
        auto start_control_point =
            last_is_cubic ? Point{last_end_point.x * 2, last_end_point.y * 2} -
                                last_control_point
                          : current_position;
        auto end_control_point = read_point();
        auto end_point = read_point();
        CubicBezierTo(start_control_point, end_control_point, end_point);
        last_is_cubic = true;
        last_control_point = end_control_point;
        last_end_point = end_point;
        break;
      }
      case 's': {
        auto current_position = GetCurrentPosition();
        auto start_control_point =
            last_is_cubic ? Point{last_end_point.x * 2, last_end_point.y * 2} -
                                last_control_point
                          : current_position;
        auto end_control_point = current_position + read_point();
        auto end_point = current_position + read_point();
        CubicBezierTo(start_control_point, end_control_point, end_point);
        last_is_cubic = true;
        last_control_point = end_control_point;
        last_end_point = end_point;
        break;
      }
      case 'Q': {
        auto control_point = read_point();
        auto end_point = read_point();
        QuadraticBezierTo(control_point, end_point);
        last_is_quad = true;
        last_control_point = control_point;
        last_end_point = end_point;
        break;
      }
      case 'q': {
        auto current_position = GetCurrentPosition();
        auto control_point = current_position + read_point();
        auto end_point = current_position + read_point();
        QuadraticBezierTo(control_point, end_point);
        last_is_quad = true;
        last_control_point = control_point;
        last_end_point = end_point;
        break;
      }
      case 'T': {
        auto current_position = GetCurrentPosition();
        auto control_point =
            last_is_quad ? Point{last_end_point.x * 2, last_end_point.y * 2} -
                               last_control_point
                         : current_position;
        auto end_point = read_point();
        QuadraticBezierTo(control_point, end_point);
        last_is_quad = true;
        last_control_point = control_point;
        last_end_point = end_point;
        break;
      }
      case 't': {
        auto current_position = GetCurrentPosition();
        auto control_point =
            last_is_quad ? Point{last_end_point.x * 2, last_end_point.y * 2} -
                               last_control_point
                         : current_position;
        auto end_point = current_position + read_point();
        QuadraticBezierTo(control_point, end_point);
        last_is_quad = true;
        last_control_point = control_point;
        last_end_point = end_point;
        break;
      }
      case 'A': {
        Point radius(read_number(), read_number());
        auto angle = read_number();
        auto large_arc_flag = read_number();
        auto sweep_flag = read_number();
        auto end = read_point();
        ArcTo(radius, angle, large_arc_flag, sweep_flag, end);
        break;
      }
      case 'a': {
        Point radius(read_number(), read_number());
        auto angle = read_number();
        auto large_arc_flag = read_number();
        auto sweep_flag = read_number();
        auto offset = read_point();
        RelativeArcTo(radius, angle, large_arc_flag, sweep_flag, offset);
        break;
      }
      case 'Z':
      case 'z':
        CloseFigure(true);
        break;
      default:
        throw Exception("Invalid svg path command.");
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
    IGraphicsFactory* factory, std::string_view path_d) {
  auto builder = factory->CreateGeometryBuilder();
  builder->ParseAndApplySvgPathData(path_d);
  return builder->Build();
}

}  // namespace cru::platform::graphics
