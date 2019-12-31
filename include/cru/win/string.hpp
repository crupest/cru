/*
Because the text encoding problem on Windows, here I write some functions
related to text encoding. The utf-8 and utf-16 conversion function is provided
by win32 api. However win32 api does not provide any function about charactor
iteration or index by code point. (At least I haven't found.) I don't use icu
because it is not easy to build it on Windows and the bundled version in Windows
(https://docs.microsoft.com/en-us/windows/win32/intl/international-components-for-unicode--icu-)
is only available after Windows 10 Creators Update.

Luckily, both utf-8 and utf-16 encoding are easy to learn and program with if we
only do simple iteration rather than do much sophisticated work about
complicated error situations. (And I learn the internal of the encoding by the
way.)
*/

#pragma once
#include "win_pre_config.hpp"

#include "cru/common/base.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

namespace cru::platform::win {
std::string ToUtf8String(const std::wstring_view& string);
std::wstring ToUtf16String(const std::string_view& string);

using CodePoint = std::int32_t;
constexpr CodePoint k_code_point_end = -1;

class TextEncodeException : public std::runtime_error {
 public:
  using runtime_error::runtime_error;
};

class Utf8Iterator : public Object {
 public:
  Utf8Iterator(const std::string_view& string) : string_(string) {}

  CRU_DEFAULT_COPY(Utf8Iterator)
  CRU_DEFAULT_MOVE(Utf8Iterator)

  ~Utf8Iterator() = default;

 public:
  void SetToHead() { position_ = 0; }

  // Advance current position and get next code point. Return k_code_point_end
  // if there is no next code unit(point). Throw TextEncodeException if decoding
  // fails.
  CodePoint Next();

  int CurrentPosition() const { return this->position_; }

 private:
  std::string_view string_;
  int position_ = 0;
};

class Utf16Iterator : public Object {
  static_assert(
      sizeof(wchar_t) == 2,
      "Emmm, according to my knowledge, wchar_t should be 2-length on "
      "Windows. If not, Utf16 will be broken.");

 public:
  Utf16Iterator(const std::wstring_view& string) : string_(string) {}

  CRU_DEFAULT_COPY(Utf16Iterator)
  CRU_DEFAULT_MOVE(Utf16Iterator)

  ~Utf16Iterator() = default;

 public:
  void SetToHead() { position_ = 0; }

  // Advance current position and get next code point. Return k_code_point_end
  // if there is no next code unit(point). Throw TextEncodeException if decoding
  // fails.
  CodePoint Next();

  int CurrentPosition() const { return this->position_; }

 private:
  std::wstring_view string_;
  int position_ = 0;
};

int IndexUtf8ToUtf16(const std::string_view& utf8_string, int utf8_index,
                     const std::wstring_view& utf16_string);

int IndexUtf16ToUtf8(const std::wstring_view& utf16_string, int utf16_index,
                     const std::string_view& utf8_string);

}  // namespace cru::platform::win
