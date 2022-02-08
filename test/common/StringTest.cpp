#include "cru/common/Format.h"
#include "cru/common/String.h"

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

TEST(StringView, ToUtf8) {
  using cru::StringView;
  StringView utf16_text = u"aπ你🤣!";
  std::string_view utf8_text = "aπ你🤣!";

  ASSERT_EQ(utf16_text.ToUtf8(), utf8_text);
}

TEST(String, FromUtf8) {
  std::u16string_view utf16_text = u"aπ你🤣!";
  std::string_view utf8_text = "aπ你🤣!";

  ASSERT_EQ(cru::String::FromUtf8(utf8_text), utf16_text);
}

TEST(StringView, ParseToDouble) {
  using cru::StringView;
  ASSERT_EQ(StringView(u"3.14159").ParseToDouble(), 3.14159);
  ASSERT_EQ(StringView(u"   3.14159").ParseToDouble(), 3.14159);
  ASSERT_EQ(StringView(u"   3.14159    ").ParseToDouble(), 3.14159);
}

TEST(String, ParseToDoubleList) {
  using cru::StringView;

  auto list = StringView(u" 1.23 2.34 3.45 ").ParseToDoubleList();

  ASSERT_EQ(list.size(), 3);
  ASSERT_EQ(list[0], 1.23);
  ASSERT_EQ(list[1], 2.34);
  ASSERT_EQ(list[2], 3.45);
}
