#include "cru/common/String.hpp"

#include <gtest/gtest.h>

TEST(String, Append) {
  using cru::String;

  String s;
  s.append(u"ha");
  s.append(s);
  ASSERT_EQ(s, String(u"haha"));
}

TEST(String, Format) {
  using cru::Format;
  using cru::String;

  ASSERT_EQ(Format(u"{} + {} = {}", 123, 321, 444), String(u"123 + 321 = 444"));
}
