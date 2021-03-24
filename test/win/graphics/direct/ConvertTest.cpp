#include "cru/platform/Matrix.hpp"
#include "cru/win/graphics/direct/ConvertUtil.hpp"

#include <gtest/gtest.h>

using cru::platform::Matrix;
using cru::platform::graphics::win::direct::Convert;

TEST(MatrixConvert, Rotation) {
  auto matrix = Convert(Matrix::Rotation(90));

  auto m = *D2D1::Matrix3x2F::ReinterpretBaseType(&matrix);

  auto p = m.TransformPoint({1, 1});

  ASSERT_FLOAT_EQ(p.x, -1);
  ASSERT_FLOAT_EQ(p.y, 1);
}

TEST(MatrixConvert, RotationAndTranslation) {
  auto matrix = Convert(Matrix::Rotation(90) * Matrix::Translation(1, 1));

  auto m = *D2D1::Matrix3x2F::ReinterpretBaseType(&matrix);

  auto p = m.TransformPoint({1, 1});

  ASSERT_FLOAT_EQ(p.x, 0);
  ASSERT_FLOAT_EQ(p.y, 2);
}
