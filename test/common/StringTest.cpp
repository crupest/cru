#include "cru/base/Format.h"
#include "cru/base/String.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("String Append", "[string]") {
  using cru::String;

  String s;
  s.append(u"ha");
  s.append(s);
  REQUIRE(s == String(u"haha"));
}

TEST_CASE("String IndexConvert", "[string]") {
  using cru::String;

  String s(u"123");
  REQUIRE(s.IndexFromCodePointToCodeUnit(1) == 1);
  REQUIRE(s.IndexFromCodeUnitToCodePoint(1) == 1);
  REQUIRE(s.IndexFromCodeUnitToCodePoint(3) == 3);
  REQUIRE(s.IndexFromCodeUnitToCodePoint(3) == 3);
}

TEST_CASE("String Format", "[string]") {
  using cru::Format;
  using cru::String;

  REQUIRE(Format(u"{} + {} = {}", 123, 321, 444) == String(u"123 + 321 = 444"));
}

TEST_CASE("String Trim", "[string]") {
  using cru::String;
  REQUIRE(String(u"  abc  ").Trim() == u"abc");
}

TEST_CASE("String SplitToLines", "[string]") {
  using cru::String;

  String s(u"abc\ndef\nghi");
  auto lines = s.SplitToLines();
  REQUIRE(lines.size() == 3);
  REQUIRE(lines[0] == String(u"abc"));
  REQUIRE(lines[1] == String(u"def"));
  REQUIRE(lines[2] == String(u"ghi"));
}

TEST_CASE("String SplitToLinesWithEmptyLine", "[string]") {
  using cru::String;

  String s(u"abc\n   \ndef\n\nghi\n");
  auto lines = s.SplitToLines();
  REQUIRE(lines.size() == 6);
  REQUIRE(lines[0] == String(u"abc"));
  REQUIRE(lines[1] == String(u"   "));
  REQUIRE(lines[2] == String(u"def"));
  REQUIRE(lines[3] == String(u""));
  REQUIRE(lines[4] == String(u"ghi"));
  REQUIRE(lines[5] == String(u""));
}

TEST_CASE("String SplitToLinesRemoveSpaceLine", "[string]") {
  using cru::String;

  String s(u"abc\n   \ndef\n\nghi\n");
  auto lines = s.SplitToLines(true);
  REQUIRE(lines.size() == 3);
  REQUIRE(lines[0] == String(u"abc"));
  REQUIRE(lines[1] == String(u"def"));
  REQUIRE(lines[2] == String(u"ghi"));
}

TEST_CASE("StringView ToUtf8", "[string]") {
  using cru::StringView;
  StringView utf16_text = u"aπ你🤣!";
  std::string_view utf8_text = "aπ你🤣!";

  REQUIRE(utf16_text.ToUtf8() == utf8_text);
}

TEST_CASE("String FromUtf8", "[string]") {
  std::u16string_view utf16_text = u"aπ你🤣!";
  std::string_view utf8_text = "aπ你🤣!";

  REQUIRE(cru::String::FromUtf8(utf8_text) == utf16_text);
}

TEST_CASE("StringView ParseToDouble", "[string]") {
  using cru::StringToNumberFlags;
  using cru::StringView;
  REQUIRE(StringView(u"3.14159").ParseToDouble() == 3.14159);
  REQUIRE(
      StringView(u"   3.14159")
          .ParseToDouble(nullptr, StringToNumberFlags::kAllowLeadingSpaces) ==
      3.14159);
  REQUIRE(StringView(u"   3.14159    ")
              .ParseToDouble(nullptr,
                             StringToNumberFlags::kAllowLeadingSpaces |
                                 StringToNumberFlags::kAllowTrailingSpaces) ==
          3.14159);
}

TEST_CASE("String ParseToDoubleList", "[string]") {
  using cru::StringView;

  auto list = StringView(u" 1.23 2.34 3.45 ").ParseToDoubleList();

  REQUIRE(list.size() == 3);
  REQUIRE(list[0] == 1.23);
  REQUIRE(list[1] == 2.34);
  REQUIRE(list[2] == 3.45);
}
