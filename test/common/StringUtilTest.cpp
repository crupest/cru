#include "cru/common/StringUtil.hpp"

#include <gtest/gtest.h>

using cru::k_invalid_code_point;

TEST(StringUtil, Utf8NextCodePoint) {
  using cru::Utf8NextCodePoint;
  std::string_view text = "aπ你🤣!";
  gsl::index current = 0;
  ASSERT_EQ(Utf8NextCodePoint(text, current, &current), 0x0061);
  ASSERT_EQ(Utf8NextCodePoint(text, current, &current), 0x03C0);
  ASSERT_EQ(Utf8NextCodePoint(text, current, &current), 0x4F60);
  ASSERT_EQ(Utf8NextCodePoint(text, current, &current), 0x1F923);
  ASSERT_EQ(Utf8NextCodePoint(text, current, &current), 0x0021);
  ASSERT_EQ(Utf8NextCodePoint(text, current, &current), k_invalid_code_point);
  ASSERT_EQ(current, static_cast<gsl::index>(text.size()));
}

TEST(StringUtil, Utf16NextCodePoint) {
  using cru::Utf16NextCodePoint;
  std::u16string_view text = u"aπ你🤣!";
  gsl::index current = 0;
  ASSERT_EQ(Utf16NextCodePoint(text, current, &current), 0x0061);
  ASSERT_EQ(Utf16NextCodePoint(text, current, &current), 0x03C0);
  ASSERT_EQ(Utf16NextCodePoint(text, current, &current), 0x4F60);
  ASSERT_EQ(Utf16NextCodePoint(text, current, &current), 0x1F923);
  ASSERT_EQ(Utf16NextCodePoint(text, current, &current), 0x0021);
  ASSERT_EQ(Utf16NextCodePoint(text, current, &current), k_invalid_code_point);
  ASSERT_EQ(current, static_cast<gsl::index>(text.size()));
}

TEST(StringUtil, Utf16PreviousCodePoint) {
  using cru::Utf16PreviousCodePoint;
  std::u16string_view text = u"aπ你🤣!";
  gsl::index current = text.size();
  ASSERT_EQ(Utf16PreviousCodePoint(text, current, &current), 0x0021);
  ASSERT_EQ(Utf16PreviousCodePoint(text, current, &current), 0x1F923);
  ASSERT_EQ(Utf16PreviousCodePoint(text, current, &current), 0x4F60);
  ASSERT_EQ(Utf16PreviousCodePoint(text, current, &current), 0x03C0);
  ASSERT_EQ(Utf16PreviousCodePoint(text, current, &current), 0x0061);
  ASSERT_EQ(Utf16PreviousCodePoint(text, current, &current),
            k_invalid_code_point);
  ASSERT_EQ(current, 0);
}

TEST(StringUtil, Utf8CodePointIterator) {
  using cru::Utf8CodePointIterator;
  std::string_view text = "aπ你🤣!";
  std::vector<cru::CodePoint> code_points;

  for (auto cp : Utf8CodePointIterator{text}) {
    code_points.push_back(cp);
  }

  std::vector<cru::CodePoint> expected_code_points{0x0061, 0x03C0, 0x4F60,
                                                   0x1F923, 0x0021};

  ASSERT_EQ(code_points, expected_code_points);
}

TEST(StringUtil, Utf16CodePointIterator) {
  using cru::Utf16CodePointIterator;
  std::u16string_view text = u"aπ你🤣!";
  std::vector<cru::CodePoint> code_points;

  for (auto cp : Utf16CodePointIterator{text}) {
    code_points.push_back(cp);
  }

  std::vector<cru::CodePoint> expected_code_points{0x0061, 0x03C0, 0x4F60,
                                                   0x1F923, 0x0021};

  ASSERT_EQ(code_points, expected_code_points);
}

TEST(StringUtil, ToUtf8) {
  using cru::ToUtf8;
  std::u16string_view utf16_text = u"aπ你🤣!";
  std::string_view utf8_text = "aπ你🤣!";

  ASSERT_EQ(ToUtf8(utf16_text), utf8_text);
}

TEST(StringUtil, ToUtf16) {
  using cru::ToUtf16;
  std::u16string_view utf16_text = u"aπ你🤣!";
  std::string_view utf8_text = "aπ你🤣!";

  ASSERT_EQ(ToUtf16(utf8_text), utf16_text);
}

// TEST(WinString, IndexUtf8ToUtf16) {
//   using cru::platform::win::IndexUtf8ToUtf16;
//   std::string_view utf8_string = "aπ你🤣!";
//   std::wstring_view utf16_string = L"aπ你🤣!";
//   ASSERT_EQ(IndexUtf8ToUtf16(utf8_string, 0, utf16_string), 0);
//   ASSERT_EQ(IndexUtf8ToUtf16(utf8_string, 1, utf16_string), 1);
//   ASSERT_EQ(IndexUtf8ToUtf16(utf8_string, 3, utf16_string), 2);
//   ASSERT_EQ(IndexUtf8ToUtf16(utf8_string, 6, utf16_string), 3);
//   ASSERT_EQ(IndexUtf8ToUtf16(utf8_string, 10, utf16_string), 5);
//   ASSERT_EQ(IndexUtf8ToUtf16(utf8_string, 11, utf16_string), 6);
// }

// TEST(WinString, IndexUtf16ToUtf8) {
//   using cru::platform::win::IndexUtf16ToUtf8;
//   std::string_view utf8_string = "aπ你🤣!";
//   std::wstring_view utf16_string = L"aπ你🤣!";
//   ASSERT_EQ(IndexUtf16ToUtf8(utf16_string, 0, utf8_string), 0);
//   ASSERT_EQ(IndexUtf16ToUtf8(utf16_string, 1, utf8_string), 1);
//   ASSERT_EQ(IndexUtf16ToUtf8(utf16_string, 2, utf8_string), 3);
//   ASSERT_EQ(IndexUtf16ToUtf8(utf16_string, 3, utf8_string), 6);
//   ASSERT_EQ(IndexUtf16ToUtf8(utf16_string, 5, utf8_string), 10);
//   ASSERT_EQ(IndexUtf16ToUtf8(utf16_string, 6, utf8_string), 11);
// }
