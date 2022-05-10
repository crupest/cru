#include "cru/platform/Matrix.h"
#include "cru/win/graphics/direct/ConvertUtil.h"

#include <catch2/catch_test_macros.hpp>

using cru::platform::Matrix;
using cru::platform::graphics::win::direct::Convert;

TEST_CASE("MatrixConvert Rotation", "[matrix]") {
  auto matrix = Convert(Matrix::Rotation(90));

  auto m = *D2D1::Matrix3x2F::ReinterpretBaseType(&matrix);

  auto p = m.TransformPoint({1, 1});

  REQUIRE(p.x == -1);
  REQUIRE(p.y == 1);
}

TEST_CASE("MatrixConvert RotationAndTranslation", "[matrix]") {
  auto matrix = Convert(Matrix::Rotation(90) * Matrix::Translation(1, 1));

  auto m = *D2D1::Matrix3x2F::ReinterpretBaseType(&matrix);

  auto p = m.TransformPoint({1, 1});

  REQUIRE(p.x == 0);
  REQUIRE(p.y == 2);
}
