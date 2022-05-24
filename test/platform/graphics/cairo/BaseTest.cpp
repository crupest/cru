#include "cru/platform/graphics/cairo/Base.h"

#include <cairo/cairo.h>
#include <catch2/catch_test_macros.hpp>
#include "catch2/catch_approx.hpp"

using Catch::Approx;

TEST_CASE("Cairo Matrix Convert", "[matrix]") {
  using namespace cru::platform;
  using namespace cru::platform::graphics;
  using namespace cru::platform::graphics::cairo;

  SECTION("Cairo to cru should work") {
    cairo_matrix_t cairo_matrix;
    cairo_matrix_init_identity(&cairo_matrix);
    cairo_matrix_scale(&cairo_matrix, 2, 2);
    cairo_matrix_rotate(&cairo_matrix, 1);
    cairo_matrix_translate(&cairo_matrix, 10, 10);
    auto cru_matrix = Convert(cairo_matrix);
    Point original_point(3, 5);
    double cairo_point_x = original_point.x, cairo_point_y = original_point.y;
    cairo_matrix_transform_point(&cairo_matrix, &cairo_point_x, &cairo_point_y);
    Point cru_point = cru_matrix.TransformPoint(original_point);
    REQUIRE(Approx(cru_point.x) == cairo_point_x);
    REQUIRE(Approx(cru_point.y) == cairo_point_y);
  }

  SECTION("Cru to cairo should work") {
    auto cru_matrix = Matrix::Identity() * Matrix::Scale(2, 2) *
                      Matrix::Rotation(60) * Matrix::Translation(10, 10);
    cairo_matrix_t cairo_matrix = Convert(cru_matrix);
    Point original_point(3, 5);
    double cairo_point_x = original_point.x, cairo_point_y = original_point.y;
    cairo_matrix_transform_point(&cairo_matrix, &cairo_point_x, &cairo_point_y);
    Point cru_point = cru_matrix.TransformPoint(original_point);
    REQUIRE(Approx(cru_point.x) == cairo_point_x);
    REQUIRE(Approx(cru_point.y) == cairo_point_y);
  }
}
