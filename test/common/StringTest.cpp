#include "cru/common/String.hpp"

#include <gtest/gtest.h>

TEST(String, Format) {
  using cru::Format;

  ASSERT_EQ(Format(u"{} + {} = {}", 123, 321, 444), u"123 + 321 = 444");
}
