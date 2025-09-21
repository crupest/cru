#include "cru/platform/graphics/Geometry.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <numbers>

using Catch::Approx;
using cru::platform::graphics::IGeometryBuilder;

constexpr float margin = 0.000001f;
constexpr float pi = std::numbers::pi_v<float>;

// Helper: normalizes an angle to [0, 2π).
static float NormAngle(float a) {
  a = std::fmod(a, 2.0f * pi);
  if (a < 0) a += 2.0f * pi;
  return a;
}

// Helper: verifies that the returned center, when transformed to unit-circle
// space, is at distance 1 from the transformed start and end points.
static void VerifyUnitCircleInvariant(const IGeometryBuilder::ArcInfo& info,
                                      float start_x, float start_y,
                                      float radius_x, float radius_y,
                                      float angle, float end_x, float end_y) {
  using cru::platform::Matrix;
  // The ellipse-to-unit-circle transform is: rotate by -angle, then scale by
  // 1/radius. With our row-vector convention that applies matrices right-to-
  // left, Scale must be multiplied first.
  auto matrix = Matrix::Scale(1.0f / radius_x, 1.0f / radius_y) *
                Matrix::Rotation(-angle);
  auto s1 = matrix.TransformPoint({start_x, start_y});
  auto s2 = matrix.TransformPoint({end_x, end_y});
  auto unit_center = matrix.TransformPoint(info.center);
  REQUIRE(unit_center.Distance(s1) == Approx(1.0f).margin(0.002f));
  REQUIRE(unit_center.Distance(s2) == Approx(1.0f).margin(0.002f));
}

TEST_CASE("IGeometryBuilder CalculateArcInfo basic", "[graphics][geometry]") {
  auto test = [](float start_x, float start_y, float radius_x, float radius_y,
                 float angle, bool is_large_arc, bool is_clockwise, float end_x,
                 float end_y, float expect_center_x, float expect_center_y,
                 float expect_start_angle, float expect_end_angle) {
    auto info = IGeometryBuilder::CalculateArcInfo(
        {start_x, start_y}, {radius_x, radius_y}, angle, is_large_arc,
        is_clockwise, {end_x, end_y});
    REQUIRE(info.center.x == Approx(expect_center_x).margin(margin));
    REQUIRE(info.center.y == Approx(expect_center_y).margin(margin));
    REQUIRE(info.radius_x == Approx(radius_x).margin(margin));
    REQUIRE(info.radius_y == Approx(radius_y).margin(margin));
    REQUIRE(NormAngle(info.start_angle) ==
            Approx(NormAngle(expect_start_angle)).margin(margin));
    REQUIRE(NormAngle(info.end_angle) ==
            Approx(NormAngle(expect_end_angle)).margin(margin));
    VerifyUnitCircleInvariant(info, start_x, start_y, radius_x, radius_y, angle,
                              end_x, end_y);
  };

  // ==================== Quarter-circle tests (unit circle, 90° apart) ======
  // Quadrant I: start=(1,0), end=(0,1)

  // Clockwise small: 90° clockwise from (1,0) to (0,1).
  test(1, 0, 1, 1, 0, false, true, 0, 1, 0, 0, 0, pi / 2);
  // Clockwise large: 270° clockwise → center on opposite side.
  test(1, 0, 1, 1, 0, true, true, 0, 1, 1, 1, -pi / 2, pi);
  // Counterclockwise small: 90° counterclockwise → center on opposite side.
  test(1, 0, 1, 1, 0, false, false, 0, 1, 1, 1, -pi / 2, pi);
  // Counterclockwise large: 270° counterclockwise.
  test(1, 0, 1, 1, 0, true, false, 0, 1, 0, 0, 0, pi / 2);

  // Quadrant II: start=(0,1), end=(-1,0)

  test(0, 1, 1, 1, 0, false, true, -1, 0, 0, 0, pi / 2, pi);
  test(0, 1, 1, 1, 0, true, true, -1, 0, -1, 1, 0, -pi / 2);
  test(0, 1, 1, 1, 0, false, false, -1, 0, -1, 1, 0, -pi / 2);
  test(0, 1, 1, 1, 0, true, false, -1, 0, 0, 0, pi / 2, pi);

  // Quadrant III: start=(-1,0), end=(0,-1)

  test(-1, 0, 1, 1, 0, false, true, 0, -1, 0, 0, pi, -pi / 2);
  test(-1, 0, 1, 1, 0, true, true, 0, -1, -1, -1, pi / 2, 0);
  test(-1, 0, 1, 1, 0, false, false, 0, -1, -1, -1, pi / 2, 0);
  test(-1, 0, 1, 1, 0, true, false, 0, -1, 0, 0, pi, -pi / 2);

  // Quadrant IV: start=(0,-1), end=(1,0)

  test(0, -1, 1, 1, 0, false, true, 1, 0, 0, 0, -pi / 2, 0);
  test(0, -1, 1, 1, 0, true, true, 1, 0, 1, -1, pi, pi / 2);
  test(0, -1, 1, 1, 0, false, false, 1, 0, 1, -1, pi, pi / 2);
  test(0, -1, 1, 1, 0, true, false, 1, 0, 0, 0, -pi / 2, 0);
}

TEST_CASE("IGeometryBuilder CalculateArcInfo non-quarter",
          "[graphics][geometry]") {
  auto test = [](float start_x, float start_y, float radius_x, float radius_y,
                 float angle, bool is_large_arc, bool is_clockwise, float end_x,
                 float end_y, float expect_center_x, float expect_center_y,
                 float expect_start_angle, float expect_end_angle) {
    auto info = IGeometryBuilder::CalculateArcInfo(
        {start_x, start_y}, {radius_x, radius_y}, angle, is_large_arc,
        is_clockwise, {end_x, end_y});
    CAPTURE(info.center.x, info.center.y, info.start_angle, info.end_angle);

    REQUIRE(info.center.x == Approx(expect_center_x).margin(margin));
    REQUIRE(info.center.y == Approx(expect_center_y).margin(margin));
    REQUIRE(info.radius_x == Approx(radius_x).margin(margin));
    REQUIRE(info.radius_y == Approx(radius_y).margin(margin));
    REQUIRE(NormAngle(info.start_angle) ==
            Approx(NormAngle(expect_start_angle)).margin(margin));
    REQUIRE(NormAngle(info.end_angle) ==
            Approx(NormAngle(expect_end_angle)).margin(margin));
    VerifyUnitCircleInvariant(info, start_x, start_y, radius_x, radius_y, angle,
                              end_x, end_y);
  };

  // ==================== 60° apart on unit circle ===========================
  // Points: (1,0) and (cos 60°, sin 60°) = (0.5, √3/2)
  float cos60 = 0.5f;
  float sin60 = std::sqrt(3.0f) / 2.0f;

  // Clockwise small: 60° clockwise from (1,0) to (0.5, 0.866).
  // Center = (0,0), a1=0, a2=π/3.
  test(1, 0, 1, 1, 0, false, true, cos60, sin60, 0, 0, 0, pi / 3);

  // Clockwise large: 300° clockwise → center on opposite side.
  // With d=0.5, dc=√0.75≈0.866.
  // a = atan2(1-0.5, 0.866-0) = atan2(0.5, 0.866) = π/6.
  // center = mid + dc*perp = (0.75,0.433) + 0.866*(cos(π/6), sin(π/6))
  //        = (0.75+0.75, 0.433+0.433) = (1.5, 0.866).
  // a1 = atan2(0-0.866, 1-1.5) = atan2(-0.866, -0.5) = -2π/3.
  // a2 = atan2(0.866-0.866, 0.5-1.5) = atan2(0, -1) = π.
  test(1, 0, 1, 1, 0, true, true, cos60, sin60, 1.5f, sin60, -2.0f * pi / 3.0f,
       pi);

  // Counterclockwise small: 60° counterclockwise.
  // After swap: s1=(0.5,0.866), s2=(1,0). sign=-1.
  // center = mid - dc*perp (same mid, perp as above but sign flipped)
  // a = atan2(0.5-1, 0-0.866) = atan2(-0.5, -0.866) = -5π/6.
  // perp = (cos(-5π/6), sin(-5π/6)) = (-0.866, -0.5).
  // center = (0.75,0.433) - 0.866*(-0.866, -0.5) = (0.75+0.75, 0.433+0.433)
  //        = (1.5, 0.866).
  // a1 = atan2(0.866-0.866, 0.5-1.5) = atan2(0, -1) = π.
  // a2 = atan2(0-0.866, 1-1.5) = atan2(-0.866, -0.5) = -2π/3.
  test(1, 0, 1, 1, 0, false, false, cos60, sin60, 1.5f, sin60,
       -2.0f * pi / 3.0f, pi);

  // Counterclockwise large: 300° counterclockwise.
  // After swap: s1=(0.5,0.866), s2=(1,0). sign=+1.
  // center = mid + dc*perp = (0.75,0.433) + 0.866*(-0.866, -0.5)
  //        = (0.75-0.75, 0.433-0.433) = (0, 0).
  // a1 = atan2(0.866, 0.5) = π/3, a2 = atan2(0, 1) = 0.
  test(1, 0, 1, 1, 0, true, false, cos60, sin60, 0, 0, 0, pi / 3);

  // ==================== Diameter case (d = 1, 180° apart) ==================
  // Both centers coincide (dc=0). Large and small arcs produce the same
  // result.
  // Clockwise: from (1,0) to (-1,0) clockwise.
  test(1, 0, 1, 1, 0, false, true, -1, 0, 0, 0, 0, pi);
  test(1, 0, 1, 1, 0, true, true, -1, 0, 0, 0, 0, pi);
  // Counterclockwise: from (1,0) to (-1,0) counterclockwise.
  // s1=(1,0), s2=(-1,0). center=(0,0). a1=0, a2=π.
  test(1, 0, 1, 1, 0, false, false, -1, 0, 0, 0, 0, pi);
  test(1, 0, 1, 1, 0, true, false, -1, 0, 0, 0, 0, pi);

  // ==================== Near-diameter (d close to 1) =======================
  // Points 179° apart: start=(1,0), end=(-cos(1°), sin(1°)) ≈ (-0.99985,
  // 0.01745)
  {
    float deg1 = pi / 180.0f;  // 1° in radians
    float ex = -std::cos(deg1);
    float ey = std::sin(deg1);
    auto info = IGeometryBuilder::CalculateArcInfo({1, 0}, {1, 1}, 0, false,
                                                   true, {ex, ey});
    VerifyUnitCircleInvariant(info, 1, 0, 1, 1, 0, ex, ey);
  }
}

TEST_CASE("IGeometryBuilder CalculateArcInfo elliptical",
          "[graphics][geometry]") {
  // Ellipse with rx=2, ry=1, no rotation.
  // Clockwise small: start=(2,0), end=(0,1).
  {
    auto info = IGeometryBuilder::CalculateArcInfo({2, 0}, {2, 1}, 0, false,
                                                   true, {0, 1});
    REQUIRE(info.center.x == Approx(0).margin(margin));
    REQUIRE(info.center.y == Approx(0).margin(margin));
    REQUIRE(info.radius_x == Approx(2).margin(margin));
    REQUIRE(info.radius_y == Approx(1).margin(margin));
    REQUIRE(info.start_angle == Approx(0).margin(margin));
    REQUIRE(info.end_angle == Approx(pi / 2).margin(margin));
    VerifyUnitCircleInvariant(info, 2, 0, 2, 1, 0, 0, 1);
  }

  // Clockwise large
  {
    auto info = IGeometryBuilder::CalculateArcInfo({2, 0}, {2, 1}, 0, true,
                                                   true, {0, 1});
    REQUIRE(info.center.x == Approx(2).margin(margin));
    REQUIRE(info.center.y == Approx(1).margin(margin));
    REQUIRE(info.radius_x == Approx(2).margin(margin));
    REQUIRE(info.radius_y == Approx(1).margin(margin));
    REQUIRE(NormAngle(info.start_angle) ==
            Approx(NormAngle(-pi / 2)).margin(margin));
    REQUIRE(NormAngle(info.end_angle) == Approx(NormAngle(pi)).margin(margin));
    VerifyUnitCircleInvariant(info, 2, 0, 2, 1, 0, 0, 1);
  }
}

TEST_CASE("IGeometryBuilder CalculateArcInfo rotated", "[graphics][geometry]") {
  // 90° rotation: start=(1,0), end=(0,1), angle=90.

  // Clockwise small, angle=90
  {
    auto info = IGeometryBuilder::CalculateArcInfo({1, 0}, {1, 1}, 90, false,
                                                   true, {0, 1});
    REQUIRE(info.center.x == Approx(0).margin(margin));
    REQUIRE(info.center.y == Approx(0).margin(margin));
    VerifyUnitCircleInvariant(info, 1, 0, 1, 1, 90, 0, 1);
  }

  // Clockwise large, angle=90
  {
    auto info = IGeometryBuilder::CalculateArcInfo({1, 0}, {1, 1}, 90, true,
                                                   true, {0, 1});
    // In unit space after Rotation(-90): s1=(0,-1), s2=(1,0).
    // Large arc center in unit space = (1, -1).
    // Transform back with Inv(Rotation(-90)) = Rotation(90): (1, 1).
    REQUIRE(info.center.x == Approx(1).margin(margin));
    REQUIRE(info.center.y == Approx(1).margin(margin));
    // a1 = atan2(-1-(-1), 0-1) = atan2(0,-1) = π
    // a2 = atan2(0-(-1), 1-1) = atan2(1,0) = π/2
    REQUIRE(NormAngle(info.start_angle) ==
            Approx(NormAngle(pi)).margin(margin));
    REQUIRE(NormAngle(info.end_angle) ==
            Approx(NormAngle(pi / 2)).margin(margin));
    VerifyUnitCircleInvariant(info, 1, 0, 1, 1, 90, 0, 1);
  }

  // Ellipse with rx=2, ry=1, rotated 90°. This exercises the full transform
  // where Scale and Rotation do not commute.
  {
    auto info = IGeometryBuilder::CalculateArcInfo({2, 0}, {2, 1}, 90, false,
                                                   true, {0, 1});
    REQUIRE(info.center.x == Approx(0).margin(margin));
    REQUIRE(info.center.y == Approx(0).margin(margin));
    REQUIRE(info.radius_x == Approx(2).margin(margin));
    REQUIRE(info.radius_y == Approx(1).margin(margin));
    REQUIRE(NormAngle(info.start_angle) ==
            Approx(NormAngle(-pi / 2)).margin(margin));
    REQUIRE(std::abs(NormAngle(info.end_angle) - 2.0f * pi) < margin);
    VerifyUnitCircleInvariant(info, 2, 0, 2, 1, 90, 0, 1);
  }
}

TEST_CASE("IGeometryBuilder CalculateArcInfo full circle",
          "[graphics][geometry]") {
  // Full circle: start == end. The returned angles should differ by 2π.

  // Clockwise
  {
    auto info = IGeometryBuilder::CalculateArcInfo({1, 0}, {1, 1}, 0, false,
                                                   true, {1, 0});
    REQUIRE(info.radius_x == Approx(1).margin(margin));
    REQUIRE(info.radius_y == Approx(1).margin(margin));
    float diff = info.end_angle - info.start_angle;
    REQUIRE(std::abs(diff - 2.0f * pi) < margin);
    VerifyUnitCircleInvariant(info, 1, 0, 1, 1, 0, 1, 0);
  }

  // Counterclockwise
  {
    auto info = IGeometryBuilder::CalculateArcInfo({1, 0}, {1, 1}, 0, false,
                                                   false, {1, 0});
    float diff = info.end_angle - info.start_angle;
    REQUIRE(std::abs(diff - 2.0f * pi) < margin);
    VerifyUnitCircleInvariant(info, 1, 0, 1, 1, 0, 1, 0);
  }

  // Full circle with elliptical radii
  {
    auto info = IGeometryBuilder::CalculateArcInfo({3, 0}, {3, 2}, 0, false,
                                                   true, {3, 0});
    REQUIRE(info.radius_x == Approx(3).margin(margin));
    REQUIRE(info.radius_y == Approx(2).margin(margin));
    float diff = info.end_angle - info.start_angle;
    REQUIRE(std::abs(diff - 2.0f * pi) < margin);
    VerifyUnitCircleInvariant(info, 3, 0, 3, 2, 0, 3, 0);
  }
}

TEST_CASE("IGeometryBuilder CalculateArcInfo radii scaling",
          "[graphics][geometry]") {
  // When the chord length exceeds 2*radius (d > 1), radii are scaled up
  // per the SVG spec so the arc becomes valid (reduces to a diameter case).

  // Distance 3, radius 1: needs scaling by d=1.5.
  {
    auto info = IGeometryBuilder::CalculateArcInfo({0, 0}, {1, 1}, 0, false,
                                                   true, {3, 0});
    // Radii scaled: 1 * 1.5 = 1.5
    REQUIRE(info.radius_x == Approx(1.5f).margin(margin));
    REQUIRE(info.radius_y == Approx(1.5f).margin(margin));
    // Center should be at (1.5, 0) – the midpoint of the scaled arc.
    REQUIRE(info.center.x == Approx(1.5f).margin(margin));
    REQUIRE(info.center.y == Approx(0).margin(margin));
    VerifyUnitCircleInvariant(info, 0, 0, 1.5f, 1.5f, 0, 3, 0);
  }

  // SVG-path-like case: chord slightly exceeds diameter due to FP precision.
  // dx=0.858, dy=0.515, dist≈1.0007 > 2*0.5=1.0
  {
    auto info = IGeometryBuilder::CalculateArcInfo(
        {6.071f, 11.242f}, {0.5f, 0.5f}, 0, true, false, {6.929f, 11.757f});
    // Radii should be scaled slightly up (from 0.5 to ~0.50035).
    REQUIRE(info.radius_x > 0.5f);
    REQUIRE(info.radius_y > 0.5f);
    REQUIRE(info.radius_x < 0.51f);
    REQUIRE(info.radius_y < 0.51f);
    VerifyUnitCircleInvariant(info, 6.071f, 11.242f, info.radius_x,
                              info.radius_y, 0, 6.929f, 11.757f);
  }
}

TEST_CASE("IGeometryBuilder CalculateArcInfo invalid", "[graphics][geometry]") {
  // Zero radius
  {
    auto info = IGeometryBuilder::CalculateArcInfo({0, 0}, {0, 1}, 0, false,
                                                   true, {1, 0});
    REQUIRE(info.center.x == Approx(0).margin(margin));
    REQUIRE(info.center.y == Approx(0).margin(margin));
    REQUIRE(info.radius_x == Approx(0).margin(margin));
    REQUIRE(info.radius_y == Approx(0).margin(margin));
  }
}
