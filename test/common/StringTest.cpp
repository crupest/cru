#include "cru/common/String.hpp"

#include <gtest/gtest.h>

TEST(String, Append) {
  using cru::String;

  String s;
  s.append(u"ha");
  s.append(s);
  ASSERT_EQ(s, String(u"haha"));
}

TEST(String, IndexConvert) {
  using cru::String;

  String s(u"123");
  ASSERT_EQ(s.IndexFromCodePointToCodeUnit(1), 1);
  ASSERT_EQ(s.IndexFromCodeUnitToCodePoint(1), 1);
  ASSERT_EQ(s.IndexFromCodeUnitToCodePoint(3), 3);
  ASSERT_EQ(s.IndexFromCodeUnitToCodePoint(3), 3);

}

TEST(String, Format) {
  using cru::Format;
  using cru::String;

  ASSERT_EQ(Format(u"{} + {} = {}", 123, 321, 444), String(u"123 + 321 = 444"));
}
