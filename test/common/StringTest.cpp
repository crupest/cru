#include "cru/common/Format.hpp"
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

TEST(String, Trim) {
  using cru::String;
  ASSERT_EQ(String(u"  abc  ").Trim(), u"abc");
}

TEST(String, SplitToLines) {
  using cru::String;

  String s(u"abc\ndef\nghi");
  auto lines = s.SplitToLines();
  ASSERT_EQ(lines.size(), 3);
  ASSERT_EQ(lines[0], String(u"abc"));
  ASSERT_EQ(lines[1], String(u"def"));
  ASSERT_EQ(lines[2], String(u"ghi"));
}

TEST(String, SplitToLinesWithEmptyLine) {
  using cru::String;

  String s(u"abc\n   \ndef\n\nghi\n");
  auto lines = s.SplitToLines();
  ASSERT_EQ(lines.size(), 6);
  ASSERT_EQ(lines[0], String(u"abc"));
  ASSERT_EQ(lines[1], String(u"   "));
  ASSERT_EQ(lines[2], String(u"def"));
  ASSERT_EQ(lines[3], String(u""));
  ASSERT_EQ(lines[4], String(u"ghi"));
  ASSERT_EQ(lines[5], String(u""));
}

TEST(String, SplitToLinesRemoveSpaceLine) {
  using cru::String;

  String s(u"abc\n   \ndef\n\nghi\n");
  auto lines = s.SplitToLines(true);
  ASSERT_EQ(lines.size(), 3);
  ASSERT_EQ(lines[0], String(u"abc"));
  ASSERT_EQ(lines[1], String(u"def"));
  ASSERT_EQ(lines[2], String(u"ghi"));
}

TEST(String, ParseToFloat) {
  using cru::String;
  ASSERT_EQ(String(u"3.14159").ParseToDouble(), 3.14159);
  ASSERT_EQ(String(u"   3.14159").ParseToDouble(), 3.14159);
  ASSERT_EQ(String(u"   3.14159    ").ParseToDouble(), 3.14159);

  cru::Index processed_char_count = 0;
  ASSERT_EQ(String(u" 3.14159 garbege").ParseToDouble(&processed_char_count),
            3.14159);
  ASSERT_EQ(processed_char_count, 9);
}
