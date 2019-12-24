#include "cru/win/string.hpp"

#include <gtest/gtest.h>

using cru::platform::win::k_code_point_end;

TEST(WinString, Utf8ShouldWork) {
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

TEST(WinString, Utf16ShouldWork) {
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

