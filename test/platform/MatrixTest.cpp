#include "cru/platform/GraphicsBase.h"
#include "cru/platform/Matrix.h"

#include <gtest/gtest.h>

using cru::platform::Matrix;
using cru::platform::Point;

TEST(Matrix, Rotation) {
  Point p(1, 1);

  Point p90 = Matrix::Rotation(90).TransformPoint(p);
  ASSERT_FLOAT_EQ(p90.x, -1);
  ASSERT_FLOAT_EQ(p90.y, 1);

  Point p180 = Matrix::Rotation(180).TransformPoint(p);
  ASSERT_FLOAT_EQ(p180.x, -1);
  ASSERT_FLOAT_EQ(p180.y, -1);

  Point p270 = Matrix::Rotation(270).TransformPoint(p);
  ASSERT_FLOAT_EQ(p270.x, 1);
  ASSERT_FLOAT_EQ(p270.y, -1);
}

TEST(Matrix, TranslationAndRotation) {
  Point p =
      (Matrix::Translation(1, 1) * Matrix::Rotation(90)).TransformPoint({1, 1});
  ASSERT_FLOAT_EQ(p.x, -2);
  ASSERT_FLOAT_EQ(p.y, 2);
}

TEST(Matrix, RotationAndTranslation) {
  Point p =
      (Matrix::Rotation(90) * Matrix::Translation(1, 1)).TransformPoint({1, 1});
  ASSERT_FLOAT_EQ(p.x, 0);
  ASSERT_FLOAT_EQ(p.y, 2);
}
