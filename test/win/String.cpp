#include "cru/win/String.hpp"

#include <gtest/gtest.h>

using cru::platform::win::k_code_point_end;

TEST(WinString, Utf8Iterator) {
  using cru::platform::win::Utf8Iterator;
  std::string_view text = "aπ你🤣!";
  Utf8Iterator i{text};
  ASSERT_EQ(i.Next(), 0x0061);
  ASSERT_EQ(i.Next(), 0x03C0);
  ASSERT_EQ(i.Next(), 0x4F60);
  ASSERT_EQ(i.Next(), 0x1F923);
  ASSERT_EQ(i.Next(), 0x0021);
  ASSERT_EQ(i.Next(), k_code_point_end);
}

TEST(WinString, Utf16Iterator) {
  using cru::platform::win::Utf16Iterator;
  std::wstring_view text = L"aπ你🤣!";
  Utf16Iterator i{text};
  ASSERT_EQ(i.Next(), 0x0061);
  ASSERT_EQ(i.Next(), 0x03C0);
  ASSERT_EQ(i.Next(), 0x4F60);
  ASSERT_EQ(i.Next(), 0x1F923);
  ASSERT_EQ(i.Next(), 0x0021);
  ASSERT_EQ(i.Next(), k_code_point_end);
}

TEST(WinString, IndexUtf8ToUtf16) {
  using cru::platform::win::IndexUtf8ToUtf16;
  std::string_view utf8_string = "aπ你🤣!";
  std::wstring_view utf16_string = L"aπ你🤣!";
  ASSERT_EQ(IndexUtf8ToUtf16(utf8_string, 0, utf16_string), 0);
  ASSERT_EQ(IndexUtf8ToUtf16(utf8_string, 1, utf16_string), 1);
  ASSERT_EQ(IndexUtf8ToUtf16(utf8_string, 3, utf16_string), 2);
  ASSERT_EQ(IndexUtf8ToUtf16(utf8_string, 6, utf16_string), 3);
  ASSERT_EQ(IndexUtf8ToUtf16(utf8_string, 10, utf16_string), 5);
  ASSERT_EQ(IndexUtf8ToUtf16(utf8_string, 11, utf16_string), 6);
}

TEST(WinString, IndexUtf16ToUtf8) {
  using cru::platform::win::IndexUtf16ToUtf8;
  std::string_view utf8_string = "aπ你🤣!";
  std::wstring_view utf16_string = L"aπ你🤣!";
  ASSERT_EQ(IndexUtf16ToUtf8(utf16_string, 0, utf8_string), 0);
  ASSERT_EQ(IndexUtf16ToUtf8(utf16_string, 1, utf8_string), 1);
  ASSERT_EQ(IndexUtf16ToUtf8(utf16_string, 2, utf8_string), 3);
  ASSERT_EQ(IndexUtf16ToUtf8(utf16_string, 3, utf8_string), 6);
  ASSERT_EQ(IndexUtf16ToUtf8(utf16_string, 5, utf8_string), 10);
  ASSERT_EQ(IndexUtf16ToUtf8(utf16_string, 6, utf8_string), 11);
}
