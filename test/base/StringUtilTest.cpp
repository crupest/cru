#include "cru/base/String.h"
#include "cru/base/StringUtil.h"

#include <catch2/catch_test_macros.hpp>

using cru::Index;
using cru::k_invalid_code_point;

TEST_CASE("StringUtil Split", "[string]") {
  using cru::Split;
  REQUIRE(Split("abc", "b") == std::vector<std::string>{"a", "c"});
  REQUIRE(Split("abcd", "bc") == std::vector<std::string>{"a", "d"});
  REQUIRE(Split("abcdbcd", "bc") == std::vector<std::string>{"a", "d", "d"});
  REQUIRE(Split("aaa", "a") == std::vector<std::string>{"", "", "", ""});
}

TEST_CASE("StringUtil Utf8NextCodePoint", "[string]") {
  using cru::Utf8NextCodePoint;
  std::string_view text = "aπ你🤣!";
  Index current = 0;
  REQUIRE(Utf8NextCodePoint(text.data(), text.size(), current, &current) ==
          0x0061);
  REQUIRE(Utf8NextCodePoint(text.data(), text.size(), current, &current) ==
          0x03C0);
  REQUIRE(Utf8NextCodePoint(text.data(), text.size(), current, &current) ==
          0x4F60);
  REQUIRE(Utf8NextCodePoint(text.data(), text.size(), current, &current) ==
          0x1F923);
  REQUIRE(Utf8NextCodePoint(text.data(), text.size(), current, &current) ==
          0x0021);
  REQUIRE(Utf8NextCodePoint(text.data(), text.size(), current, &current) ==
          k_invalid_code_point);
  REQUIRE(current == static_cast<Index>(text.size()));
}

TEST_CASE("StringUtil Utf16NextCodePoint", "[string]") {
  using cru::Utf16NextCodePoint;
  std::u16string_view text = u"aπ你🤣!";
  Index current = 0;
  REQUIRE(Utf16NextCodePoint(text.data(), text.size(), current, &current) ==
          0x0061);
  REQUIRE(Utf16NextCodePoint(text.data(), text.size(), current, &current) ==
          0x03C0);
  REQUIRE(Utf16NextCodePoint(text.data(), text.size(), current, &current) ==
          0x4F60);
  REQUIRE(Utf16NextCodePoint(text.data(), text.size(), current, &current) ==
          0x1F923);
  REQUIRE(Utf16NextCodePoint(text.data(), text.size(), current, &current) ==
          0x0021);
  REQUIRE(Utf16NextCodePoint(text.data(), text.size(), current, &current) ==
          k_invalid_code_point);
  REQUIRE(current == static_cast<Index>(text.size()));
}

TEST_CASE("StringUtil Utf16PreviousCodePoint", "[string]") {
  using cru::Utf16PreviousCodePoint;
  std::u16string_view text = u"aπ你🤣!";
  Index current = text.size();
  REQUIRE(Utf16PreviousCodePoint(text.data(), text.size(), current, &current) ==
          0x0021);
  REQUIRE(Utf16PreviousCodePoint(text.data(), text.size(), current, &current) ==
          0x1F923);
  REQUIRE(Utf16PreviousCodePoint(text.data(), text.size(), current, &current) ==
          0x4F60);
  REQUIRE(Utf16PreviousCodePoint(text.data(), text.size(), current, &current) ==
          0x03C0);
  REQUIRE(Utf16PreviousCodePoint(text.data(), text.size(), current, &current) ==
          0x0061);
  REQUIRE(Utf16PreviousCodePoint(text.data(), text.size(), current, &current) ==
          k_invalid_code_point);
  REQUIRE(current == 0);
}

TEST_CASE("StringUtil Utf8CodePointIterator", "[string]") {
  using cru::Utf8CodePointIterator;
  std::string_view text = "aπ你🤣!";
  std::vector<cru::CodePoint> code_points;

  for (auto cp : Utf8CodePointIterator(text.data(), text.size())) {
    code_points.push_back(cp);
  }

  std::vector<cru::CodePoint> expected_code_points{0x0061, 0x03C0, 0x4F60,
                                                   0x1F923, 0x0021};

  REQUIRE(code_points == expected_code_points);
}

TEST_CASE("StringUtil Utf16CodePointIterator", "[string]") {
  using cru::Utf16CodePointIterator;
  std::u16string_view text = u"aπ你🤣!";
  std::vector<cru::CodePoint> code_points;

  for (auto cp : Utf16CodePointIterator(text.data(), text.size())) {
    code_points.push_back(cp);
  }

  std::vector<cru::CodePoint> expected_code_points{0x0061, 0x03C0, 0x4F60,
                                                   0x1F923, 0x0021};

  REQUIRE(code_points == expected_code_points);
}

// TEST(WinString, IndexUtf8ToUtf16) {
//   using cru::platform::win::IndexUtf8ToUtf16;
//   std::string_view utf8_string = "aπ你🤣!";
//   std::wstring_view utf16_string = L"aπ你🤣!";
//   REQUIRE(IndexUtf8ToUtf16(utf8_string, 0, utf16_string), 0);
//   REQUIRE(IndexUtf8ToUtf16(utf8_string, 1, utf16_string), 1);
//   REQUIRE(IndexUtf8ToUtf16(utf8_string, 3, utf16_string), 2);
//   REQUIRE(IndexUtf8ToUtf16(utf8_string, 6, utf16_string), 3);
//   REQUIRE(IndexUtf8ToUtf16(utf8_string, 10, utf16_string), 5);
//   REQUIRE(IndexUtf8ToUtf16(utf8_string, 11, utf16_string), 6);
// }

// TEST(WinString, IndexUtf16ToUtf8) {
//   using cru::platform::win::IndexUtf16ToUtf8;
//   std::string_view utf8_string = "aπ你🤣!";
//   std::wstring_view utf16_string = L"aπ你🤣!";
//   REQUIRE(IndexUtf16ToUtf8(utf16_string, 0, utf8_string), 0);
//   REQUIRE(IndexUtf16ToUtf8(utf16_string, 1, utf8_string), 1);
//   REQUIRE(IndexUtf16ToUtf8(utf16_string, 2, utf8_string), 3);
//   REQUIRE(IndexUtf16ToUtf8(utf16_string, 3, utf8_string), 6);
//   REQUIRE(IndexUtf16ToUtf8(utf16_string, 5, utf8_string), 10);
//   REQUIRE(IndexUtf16ToUtf8(utf16_string, 6, utf8_string), 11);
// }
