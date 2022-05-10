#include "cru/platform/GraphicsBase.h"
#include "cru/platform/Matrix.h"

#include <catch2/catch_test_macros.hpp>
#include "catch2/catch_approx.hpp"

using Catch::Approx;
using cru::platform::Matrix;
using cru::platform::Point;

TEST_CASE("Matrix Rotation", "[matrix]") {
  Point p(1, 1);

  Point p90 = Matrix::Rotation(90).TransformPoint(p);
  REQUIRE(p90.x == Approx(-1));
  REQUIRE(p90.y == Approx(1));

  Point p180 = Matrix::Rotation(180).TransformPoint(p);
  REQUIRE(p180.x == Approx(-1));
  REQUIRE(p180.y == Approx(-1));

  Point p270 = Matrix::Rotation(270).TransformPoint(p);
  REQUIRE(p270.x == Approx(1));
  REQUIRE(p270.y == Approx(-1));
}

TEST_CASE("Matrix TranslationAndRotation", "[matrix]") {
  Point p =
      (Matrix::Translation(1, 1) * Matrix::Rotation(90)).TransformPoint({1, 1});
  REQUIRE(p.x == Approx(-2));
  REQUIRE(p.y == Approx(2));
}

TEST_CASE("Matrix RotationAndTranslation", "[matrix]") {
  Point p =
      (Matrix::Rotation(90) * Matrix::Translation(1, 1)).TransformPoint({1, 1});
  REQUIRE(p.x == Approx(0));
  REQUIRE(p.y == Approx(2));
}
