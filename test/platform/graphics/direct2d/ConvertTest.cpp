#include "cru/platform/Matrix.h"
#include "cru/platform/graphics/direct2d/ConvertUtil.h"

#include <catch2/catch_test_macros.hpp>
#include "catch2/catch_approx.hpp"

using Catch::Approx;
using cru::platform::Matrix;
using cru::platform::graphics::direct2d::Convert;

TEST_CASE("MatrixConvert Rotation", "[matrix]") {
  auto matrix = Convert(Matrix::Rotation(90));

  auto m = *D2D1::Matrix3x2F::ReinterpretBaseType(&matrix);

  auto p = m.TransformPoint({1, 1});

  REQUIRE(p.x == Approx(-1));
  REQUIRE(p.y == Approx(1));
}

TEST_CASE("MatrixConvert RotationAndTranslation", "[matrix]") {
  auto matrix = Convert(Matrix::Rotation(90) * Matrix::Translation(1, 1));

  auto m = *D2D1::Matrix3x2F::ReinterpretBaseType(&matrix);

  auto p = m.TransformPoint({1, 1});

  REQUIRE(p.x == Approx(0));
  REQUIRE(p.y == Approx(2));
}
