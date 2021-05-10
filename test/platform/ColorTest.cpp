#include "cru/platform/Color.hpp"

#include <gtest/gtest.h>

using cru::platform::Color;

TEST(Color, Parse) {
  ASSERT_EQ(Color::Parse(u"blue"), Color::Parse(u"#0000ff"));
  ASSERT_EQ(Color::Parse(u"#12abAB"), Color::FromHex(0x12abAB));
  ASSERT_EQ(Color::Parse(u"#8812abAB"), Color::FromHexAlpha(0x8812abAB));
  ASSERT_EQ(Color::Parse(u"averystrangestring"), std::nullopt);
  ASSERT_EQ(Color::Parse(u"112233"), std::nullopt);
  ASSERT_EQ(Color::Parse(u"#7777777"), std::nullopt);
}
