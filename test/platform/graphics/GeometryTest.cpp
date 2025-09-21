#include "cru/platform/graphics/Geometry.h"

#include <catch2/catch_test_macros.hpp>
#include <numbers>
#include "catch2/catch_approx.hpp"

using Catch::Approx;
using cru::platform::graphics::IGeometryBuilder;

constexpr float margin = 0.000001;

TEST_CASE("IGeometryBuilder CalculateArcInfo", "[graphics][geometry]") {
  auto test = [](float start_x, float start_y, float radius_x, float radius_y,
                 float angle, bool is_large_arc, bool is_clockwise, float end_x,
                 float end_y, float expect_center_x, float expect_center_y,
                 float expect_start_angle, float expect_end_angle) {
    auto info = IGeometryBuilder::CalculateArcInfo(
        {start_x, start_y}, {radius_x, radius_y}, angle, is_large_arc,
        is_clockwise, {end_x, end_y});
    REQUIRE(info.center.x == Approx(expect_center_x).margin(margin));
    REQUIRE(info.center.y == Approx(expect_center_y).margin(margin));
    REQUIRE(info.start_angle == Approx(expect_start_angle).margin(margin));
    REQUIRE(info.end_angle == Approx(expect_end_angle).margin(margin));
  };

  test(1, 0, 1, 1, 0, false, true, 0, 1, 0, 0, 0, std::numbers::pi / 2);
  test(1, 0, 1, 1, 0, true, true, 0, 1, 0, 0, std::numbers::pi / 2, 0);
  test(1, 0, 1, 1, 0, false, false, 0, 1, 1, 1, std::numbers::pi,
       -std::numbers::pi / 2);
  test(1, 0, 1, 1, 0, true, false, 0, 1, 1, 1, -std::numbers::pi / 2,
       std::numbers::pi);

  test(0, 1, 1, 1, 0, false, true, -1, 0, 0, 0, std::numbers::pi / 2,
       std::numbers::pi);
  test(0, 1, 1, 1, 0, true, true, -1, 0, 0, 0, std::numbers::pi,
       std::numbers::pi / 2);
  test(0, 1, 1, 1, 0, false, false, -1, 0, -1, 1, -std::numbers::pi / 2, 0);
  test(0, 1, 1, 1, 0, true, false, -1, 0, -1, 1, 0, -std::numbers::pi / 2);

  test(-1, 0, 1, 1, 0, false, true, 0, -1, 0, 0, std::numbers::pi,
       -std::numbers::pi / 2);
  test(-1, 0, 1, 1, 0, true, true, 0, -1, 0, 0, -std::numbers::pi / 2,
       std::numbers::pi);
  test(-1, 0, 1, 1, 0, false, false, 0, -1, -1, -1, 0, std::numbers::pi / 2);
  test(-1, 0, 1, 1, 0, true, false, 0, -1, -1, -1, std::numbers::pi / 2, 0);

  test(0, -1, 1, 1, 0, false, true, 1, 0, 0, 0, -std::numbers::pi / 2, 0);
  test(0, -1, 1, 1, 0, true, true, 1, 0, 0, 0, 0, -std::numbers::pi / 2);
  test(0, -1, 1, 1, 0, false, false, 1, 0, 1, -1, std::numbers::pi / 2,
       std::numbers::pi);
  test(0, -1, 1, 1, 0, true, false, 1, 0, 1, -1, std::numbers::pi,
       std::numbers::pi / 2);
}
