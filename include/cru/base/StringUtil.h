#pragma once
#include "Base.h"
#include "Bitmask.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cmath>
#include <compare>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <functional>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <vector>

namespace cru::string {
class CRU_BASE_API TextEncodeException : public Exception {
 public:
  using Exception::Exception;
};

std::weak_ordering CRU_BASE_API CaseInsensitiveCompare(std::string_view left,
                                                       std::string_view right);
std::string CRU_BASE_API TrimBegin(std::string_view str);
std::string CRU_BASE_API TrimEnd(std::string_view str);
std::string CRU_BASE_API Trim(std::string_view str);
bool CRU_BASE_API IsSpace(std::string_view str);

namespace details {
struct SplitOptionsTag {};
}  // namespace details
using SplitOption = Bitmask<details::SplitOptionsTag>;
struct SplitOptions {
  static constexpr SplitOption RemoveEmpty = SplitOption::FromOffset(1);
  static constexpr SplitOption RemoveSpace = SplitOption::FromOffset(2);
};

template <typename R>
std::string Join(std::string_view sep, const R& range) {
  bool start = true;
  std::string result;
  for (const auto& s : range) {
    if (start) {
      result += s;
      start = false;
    } else {
      result += sep;
      result += s;
    }
  }
  return result;
}

std::vector<std::string> CRU_BASE_API Split(std::string_view str,
                                            std::string_view sep,
                                            SplitOption options = {});

namespace details {
template <typename T>
std::enable_if_t<std::is_integral_v<T>, std::from_chars_result> from_chars(
    const char* first, const char* last, T& value, int base = 10) {
  return std::from_chars(first, last, value, base);
}

template <typename T, T (*StrToFunc)(const char* str, char** str_end),
          bool (*IsOverflow)(T value)>
std::enable_if_t<std::is_floating_point_v<T>, std::from_chars_result>
float_from_chars_impl(const char* first, const char* last, T& value,
                      std::chars_format fmt = std::chars_format::general) {
  if (std::isspace(*first)) {
    return std::from_chars_result{first, std::errc::invalid_argument};
  }

  std::string str(first, last);
  auto c_str = str.c_str();
  char* c_str_end;
  auto parsed_value = StrToFunc(c_str, &c_str_end);
  if (c_str == c_str_end) {
    return std::from_chars_result{first, std::errc::invalid_argument};
  }
  if (IsOverflow(parsed_value)) {
    return std::from_chars_result{first, std::errc::result_out_of_range};
  }
  value = parsed_value;
  return std::from_chars_result{first + (c_str_end - c_str), {}};
}

#define CRU_DEFINE_FLOAT_FROM_CHARS(type, str_to_func, overflow_value)    \
  inline std::from_chars_result from_chars(                               \
      const char* first, const char* last, type& value,                   \
      std::chars_format fmt = std::chars_format::general) {               \
    return float_from_chars_impl<type, std::str_to_func, [](type value) { \
      return value == overflow_value;                                     \
    }>(first, last, value, fmt);                                          \
  }

CRU_DEFINE_FLOAT_FROM_CHARS(float, strtof, HUGE_VALF)
CRU_DEFINE_FLOAT_FROM_CHARS(double, strtod, HUGE_VAL)
CRU_DEFINE_FLOAT_FROM_CHARS(long double, strtold, HUGE_VALL)

#undef CRU_DEFINE_FLOAT_FROM_CHARS
}  // namespace details

namespace details {
struct ParseToNumberFlagTag {};
}  // namespace details

using ParseToNumberFlag = Bitmask<details::ParseToNumberFlagTag>;

struct ParseToNumberFlags {
  constexpr static ParseToNumberFlag AllowLeadingSpaces =
      ParseToNumberFlag::FromOffset(1);
  constexpr static ParseToNumberFlag AllowTrailingSpaces =
      ParseToNumberFlag::FromOffset(2);
  constexpr static ParseToNumberFlag AllowTrailingJunk =
      ParseToNumberFlag::FromOffset(3);
};

template <typename T>
struct ParseToNumberResult {
  bool valid;
  T value;
  Index processed_char_count;
  std::string message;
};

template <typename T>
ParseToNumberResult<T> ParseToNumber(std::string_view str,
                                     ParseToNumberFlag flags = {}) {
  ParseToNumberResult<T> result{};

  const char* ptr = str.data();
  const char* const begin = str.data();
  const char* const end = str.data() + str.size();
  if (flags.Has(ParseToNumberFlags::AllowLeadingSpaces)) {
    while (ptr != str.data() + str.size() && isspace(*ptr)) {
      ptr++;
    }
  }

  if (ptr == end) {
    result.valid = false;
    result.message =
        "Parsing reached the end (after reading all leading spaces).";
    return result;
  }

  auto parse_result = ::cru::string::details::from_chars(
      ptr, str.data() + str.size(), result.value);
  if (parse_result.ec == std::errc::invalid_argument) {
    result.valid = false;
    result.message = "Not a valid number.";
    return result;
  } else if (parse_result.ec == std::errc::result_out_of_range) {
    result.valid = false;
    result.message = "Value out of range.";
    return result;
  } else {
    if (parse_result.ptr == end ||
        flags.Has(ParseToNumberFlags::AllowTrailingJunk) ||
        (flags.Has(ParseToNumberFlags::AllowTrailingSpaces) &&
         IsSpace(std::string_view(parse_result.ptr, end)))) {
      result.valid = true;
      result.processed_char_count = parse_result.ptr - str.data();
      return result;
    } else {
      result.valid = false;
      result.message = "There are junk trailing characters.";
      return result;
    }
  }
}

template <typename T>
std::vector<T> ParseToNumberList(std::string_view str,
                                 std::string_view separator = " ") {
  auto segs = Split(str, separator, SplitOptions::RemoveSpace);
  std::vector<T> result;
  for (const auto& seg : segs) {
    auto r = ParseToNumber<T>(Trim(seg),
                              ParseToNumberFlags::AllowLeadingSpaces |
                                  ParseToNumberFlags::AllowTrailingSpaces);
    result.push_back(r.value);
  }
  return result;
}

template <typename T>
struct ImplementFormatterByToString {
  template <class ParseContext>
  constexpr ParseContext::iterator parse(ParseContext& ctx) const {
    auto iter = ctx.begin();
    if (*iter != '}') {
      throw std::format_error(
          "ImplementFormatterByToString does not accept format args.");
    }
    return iter;
  }

  template <class FmtContext>
  FmtContext::iterator format(const T& object, FmtContext& ctx) const {
    return std::ranges::copy(object.ToString(), ctx.out()).out;
  }
};

using CodePoint = std::int32_t;
using Utf8CodeUnit = char;
using Utf8StringView = std::string_view;
using Utf16CodeUnit = char16_t;
using Utf16StringView = std::u16string_view;
constexpr CodePoint k_invalid_code_point = -1;

inline bool IsUtf8LeadingByte(Utf8CodeUnit c) {
  return !(c & 0b10000000) || c & 0b01000000;
}

inline bool IsUtf8FollowingByte(Utf8CodeUnit c) {
  return !IsUtf8LeadingByte(c);
}

inline bool IsUtf16SurrogatePairCodeUnit(Utf16CodeUnit c) {
  return c >= 0xD800 && c <= 0xDFFF;
}

inline bool IsUtf16SurrogatePairLeading(Utf16CodeUnit c) {
  return c >= 0xD800 && c <= 0xDBFF;
}

inline bool IsUtf16SurrogatePairTrailing(Utf16CodeUnit c) {
  return c >= 0xDC00 && c <= 0xDFFF;
}

CodePoint CRU_BASE_API Utf8NextCodePoint(const Utf8CodeUnit* ptr, Index size,
                                         Index current, Index* next_position);

inline CodePoint Utf8NextCodePoint(Utf8StringView str, Index current,
                                   Index* next_position) {
  return Utf8NextCodePoint(str.data(), str.size(), current, next_position);
}

CodePoint CRU_BASE_API Utf8PreviousCodePoint(const Utf8CodeUnit* ptr,
                                             Index size, Index current,
                                             Index* previous_position);

inline CodePoint Utf8PreviousCodePoint(Utf8StringView str, Index current,
                                       Index* next_position) {
  return Utf8PreviousCodePoint(str.data(), str.size(), current, next_position);
}

namespace details {
template <typename Integer, int number_of_bit, typename ReturnType>
inline ReturnType ExtractBits(Integer n) {
  return static_cast<ReturnType>(n & ((1u << number_of_bit) - 1));
}
}  // namespace details

template <typename CharWriter>
std::enable_if_t<std::is_invocable_v<CharWriter, Utf8CodeUnit>, bool>
Utf8EncodeCodePointAppend(CodePoint code_point, CharWriter&& writer) {
  auto write_continue_byte = [&writer](Utf8CodeUnit byte6) {
    writer((1u << 7) + (((1u << 6) - 1) & byte6));
  };

  if (code_point >= 0 && code_point <= 0x007F) {
    writer(static_cast<Utf8CodeUnit>(code_point));
    return true;
  } else if (code_point >= 0x0080 && code_point <= 0x07FF) {
    std::uint32_t unsigned_code_point = code_point;
    writer(static_cast<Utf8CodeUnit>(
        details::ExtractBits<std::uint32_t, 5, Utf8CodeUnit>(
            (unsigned_code_point >> 6)) +
        0b11000000));
    write_continue_byte(details::ExtractBits<std::uint32_t, 6, Utf8CodeUnit>(
        unsigned_code_point));
    return true;
  } else if (code_point >= 0x0800 && code_point <= 0xFFFF) {
    std::uint32_t unsigned_code_point = code_point;
    writer(static_cast<Utf8CodeUnit>(
        details::ExtractBits<std::uint32_t, 4, Utf8CodeUnit>(
            (unsigned_code_point >> (6 * 2))) +
        0b11100000));
    write_continue_byte(details::ExtractBits<std::uint32_t, 6, Utf8CodeUnit>(
        unsigned_code_point >> 6));
    write_continue_byte(details::ExtractBits<std::uint32_t, 6, Utf8CodeUnit>(
        unsigned_code_point));
    return true;
  } else if (code_point >= 0x10000 && code_point <= 0x10FFFF) {
    std::uint32_t unsigned_code_point = code_point;
    writer(static_cast<Utf8CodeUnit>(
        details::ExtractBits<std::uint32_t, 3, Utf8CodeUnit>(
            (unsigned_code_point >> (6 * 3))) +
        0b11110000));
    write_continue_byte(details::ExtractBits<std::uint32_t, 6, Utf8CodeUnit>(
        unsigned_code_point >> (6 * 2)));
    write_continue_byte(details::ExtractBits<std::uint32_t, 6, Utf8CodeUnit>(
        unsigned_code_point >> 6));
    write_continue_byte(details::ExtractBits<std::uint32_t, 6, Utf8CodeUnit>(
        unsigned_code_point));
    return true;
  } else {
    return false;
  }
}

bool CRU_BASE_API Utf8IsValidInsertPosition(const Utf8CodeUnit* ptr, Index size,
                                            Index position);

inline bool Utf8IsValidInsertPosition(Utf8StringView str, Index position) {
  return Utf8IsValidInsertPosition(str.data(), str.size(), position);
}

// Return position after the character making predicate returns true or 0 if no
// character doing so.
Index CRU_BASE_API
Utf8BackwardUntil(const Utf8CodeUnit* ptr, Index size, Index position,
                  const std::function<bool(CodePoint)>& predicate);

inline Index Utf8BackwardUntil(
    Utf8StringView str, Index position,
    const std::function<bool(CodePoint)>& predicate) {
  return Utf8BackwardUntil(str.data(), str.size(), position, predicate);
}

// Return position before the character making predicate returns true or
// str.size() if no character doing so.
Index CRU_BASE_API
Utf8ForwardUntil(const Utf8CodeUnit* ptr, Index size, Index position,
                 const std::function<bool(CodePoint)>& predicate);

inline Index Utf8ForwardUntil(Utf8StringView str, Index position,
                              const std::function<bool(CodePoint)>& predicate) {
  return Utf8ForwardUntil(str.data(), str.size(), position, predicate);
}

Index CRU_BASE_API Utf8PreviousWord(const Utf8CodeUnit* ptr, Index size,
                                    Index position, bool* is_space = nullptr);

inline Index Utf8PreviousWord(Utf8StringView str, Index position,
                              bool* is_space = nullptr) {
  return Utf8PreviousWord(str.data(), str.size(), position, is_space);
}

Index CRU_BASE_API Utf8NextWord(const Utf8CodeUnit* ptr, Index size,
                                Index position, bool* is_space = nullptr);

inline Index Utf8NextWord(Utf8StringView str, Index position,
                          bool* is_space = nullptr) {
  return Utf8NextWord(str.data(), str.size(), position, is_space);
}

CodePoint CRU_BASE_API Utf16NextCodePoint(const Utf16CodeUnit* ptr, Index size,
                                          Index current, Index* next_position);

inline CodePoint Utf16NextCodePoint(Utf16StringView str, Index current,
                                    Index* next_position) {
  return Utf16NextCodePoint(str.data(), str.size(), current, next_position);
}

#ifdef _WIN32
inline CodePoint Utf16NextCodePoint(const wchar_t* ptr, Index size,
                                    Index current, Index* next_position) {
  return Utf16NextCodePoint(reinterpret_cast<const Utf16CodeUnit*>(ptr), size,
                            current, next_position);
}

inline CodePoint Utf16NextCodePoint(std::wstring_view str, Index current,
                                    Index* next_position) {
  return Utf16NextCodePoint(str.data(), str.size(), current, next_position);
}
#endif

CodePoint CRU_BASE_API Utf16PreviousCodePoint(const Utf16CodeUnit* ptr,
                                              Index size, Index current,
                                              Index* previous_position);

inline CodePoint Utf16PreviousCodePoint(Utf16StringView str, Index current,
                                        Index* next_position) {
  return Utf16PreviousCodePoint(str.data(), str.size(), current, next_position);
}

#ifdef _WIN32
inline CodePoint Utf16PreviousCodePoint(const wchar_t* ptr, Index size,
                                        Index current, Index* next_position) {
  return Utf16PreviousCodePoint(reinterpret_cast<const Utf16CodeUnit*>(ptr),
                                size, current, next_position);
}

inline CodePoint Utf16PreviousCodePoint(std::wstring_view str, Index current,
                                        Index* next_position) {
  return Utf16PreviousCodePoint(str.data(), str.size(), current, next_position);
}
#endif

template <typename CharWriter>
std::enable_if_t<std::is_invocable_v<CharWriter, Utf16CodeUnit>, bool>
Utf16EncodeCodePointAppend(CodePoint code_point, CharWriter&& writer) {
  if ((code_point >= 0 && code_point <= 0xD7FF) ||
      (code_point >= 0xE000 && code_point <= 0xFFFF)) {
    writer(static_cast<Utf16CodeUnit>(code_point));
    return true;
  } else if (code_point >= 0x10000 && code_point <= 0x10FFFF) {
    std::uint32_t u = code_point - 0x10000;
    writer(static_cast<Utf16CodeUnit>(
        details::ExtractBits<std::uint32_t, 10, std::uint32_t>(u >> 10) +
        0xD800u));
    writer(static_cast<Utf16CodeUnit>(
        details::ExtractBits<std::uint32_t, 10, std::uint32_t>(u) + 0xDC00u));
    return true;
  } else {
    return false;
  }
}

// If given s is not a valid utf16 string, return value is UD.
bool CRU_BASE_API Utf16IsValidInsertPosition(const Utf16CodeUnit* ptr,
                                             Index size, Index position);

inline bool Utf16IsValidInsertPosition(Utf16StringView str, Index position) {
  return Utf16IsValidInsertPosition(str.data(), str.size(), position);
}

#ifdef _WIN32
inline bool Utf16IsValidInsertPosition(const wchar_t* ptr, Index size,
                                       Index position) {
  return Utf16IsValidInsertPosition(reinterpret_cast<const Utf16CodeUnit*>(ptr),
                                    size, position);
}

inline CodePoint Utf16IsValidInsertPosition(std::wstring_view str,
                                            Index position) {
  return Utf16IsValidInsertPosition(str.data(), str.size(), position);
}
#endif

// Return position after the character making predicate returns true or 0 if no
// character doing so.
Index CRU_BASE_API
Utf16BackwardUntil(const Utf16CodeUnit* ptr, Index size, Index position,
                   const std::function<bool(CodePoint)>& predicate);

inline Index Utf16BackwardUntil(
    Utf16StringView str, Index position,
    const std::function<bool(CodePoint)>& predicate) {
  return Utf16BackwardUntil(str.data(), str.size(), position, predicate);
}

#ifdef _WIN32
inline CodePoint Utf16BackwardUntil(
    const wchar_t* ptr, Index size, Index position,
    const std::function<bool(CodePoint)>& predicate) {
  return Utf16BackwardUntil(reinterpret_cast<const Utf16CodeUnit*>(ptr), size,
                            position, predicate);
}

inline CodePoint Utf16BackwardUntil(
    std::wstring_view str, Index position,
    const std::function<bool(CodePoint)>& predicate) {
  return Utf16BackwardUntil(str.data(), str.size(), position, predicate);
}
#endif

// Return position before the character making predicate returns true or
// str.size() if no character doing so.
Index CRU_BASE_API
Utf16ForwardUntil(const Utf16CodeUnit* ptr, Index size, Index position,
                  const std::function<bool(CodePoint)>& predicate);

inline Index Utf16ForwardUntil(
    Utf16StringView str, Index position,
    const std::function<bool(CodePoint)>& predicate) {
  return Utf16ForwardUntil(str.data(), str.size(), position, predicate);
}

#ifdef _WIN32
inline CodePoint Utf16ForwardUntil(
    const wchar_t* ptr, Index size, Index position,
    const std::function<bool(CodePoint)>& predicate) {
  return Utf16ForwardUntil(reinterpret_cast<const Utf16CodeUnit*>(ptr), size,
                           position, predicate);
}

inline CodePoint Utf16ForwardUntil(
    std::wstring_view str, Index position,
    const std::function<bool(CodePoint)>& predicate) {
  return Utf16ForwardUntil(str.data(), str.size(), position, predicate);
}
#endif

Index CRU_BASE_API Utf16PreviousWord(const Utf16CodeUnit* ptr, Index size,
                                     Index position, bool* is_space = nullptr);

inline Index Utf16PreviousWord(Utf16StringView str, Index position,
                               bool* is_space = nullptr) {
  return Utf16PreviousWord(str.data(), str.size(), position, is_space);
}

#ifdef _WIN32
inline CodePoint Utf16PreviousWord(const wchar_t* ptr, Index size,
                                   Index position, bool* is_space = nullptr) {
  return Utf16PreviousWord(reinterpret_cast<const Utf16CodeUnit*>(ptr), size,
                           position, is_space);
}

inline CodePoint Utf16PreviousWord(std::wstring_view str, Index position,
                                   bool* is_space = nullptr) {
  return Utf16PreviousWord(str.data(), str.size(), position, is_space);
}
#endif

Index CRU_BASE_API Utf16NextWord(const Utf16CodeUnit* ptr, Index size,
                                 Index position, bool* is_space = nullptr);

inline Index Utf16NextWord(Utf16StringView str, Index position,
                           bool* is_space = nullptr) {
  return Utf16NextWord(str.data(), str.size(), position, is_space);
}

#ifdef _WIN32
inline CodePoint Utf16NextWord(const wchar_t* ptr, Index size, Index position,
                               bool* is_space = nullptr) {
  return Utf16NextWord(reinterpret_cast<const Utf16CodeUnit*>(ptr), size,
                       position, is_space);
}

inline CodePoint Utf16NextWord(std::wstring_view str, Index position,
                               bool* is_space = nullptr) {
  return Utf16NextWord(str.data(), str.size(), position, is_space);
}
#endif

template <typename CharType>
using NextCodePointFunctionType = CodePoint (*)(const CharType*, Index, Index,
                                                Index*);

template <typename CharType,
          NextCodePointFunctionType<CharType> NextCodePointFunction>
class CodePointIterator {
 public:
  using difference_type = Index;
  using value_type = CodePoint;
  using pointer = void;
  using reference = value_type;
  using iterator_category = std::forward_iterator_tag;

 public:
  struct past_end_tag_t {};

  explicit CodePointIterator(const CharType* ptr, Index size, Index current = 0)
      : ptr_(ptr), size_(size), position_(current) {}
  explicit CodePointIterator(const CharType* ptr, Index size, past_end_tag_t)
      : ptr_(ptr), size_(size), position_(size) {}

  CRU_DEFAULT_COPY(CodePointIterator)
  CRU_DEFAULT_MOVE(CodePointIterator)

  ~CodePointIterator() = default;

 public:
  const CharType* GetPtr() const { return ptr_; }
  Index GetSize() const { return size_; }
  Index GetPosition() const { return position_; }

  bool IsPastEnd() const { return position_ == static_cast<Index>(size_); }

 public:
  CodePointIterator begin() const { return *this; }
  CodePointIterator end() const {
    return CodePointIterator{ptr_, size_, past_end_tag_t{}};
  }

 public:
  bool operator==(const CodePointIterator& other) const {
    // You should compare iterator that iterate on the same string.
    Expects(this->ptr_ == other.ptr_ && this->size_ == other.size_);
    return this->position_ == other.position_;
  }
  bool operator!=(const CodePointIterator& other) const {
    return !this->operator==(other);
  }

  CodePointIterator& operator++() {
    Expects(!IsPastEnd());
    Forward();
    return *this;
  }

  CodePointIterator operator++(int) {
    Expects(!IsPastEnd());
    CodePointIterator old = *this;
    Forward();
    return old;
  }

  CodePoint operator*() const {
    return NextCodePointFunction(ptr_, size_, position_, &next_position_cache_);
  }

 private:
  void Forward() {
    if (next_position_cache_ > position_) {
      position_ = next_position_cache_;
    } else {
      NextCodePointFunction(ptr_, size_, position_, &position_);
    }
  }

 private:
  const CharType* ptr_;
  Index size_;
  Index position_;
  mutable Index next_position_cache_ = 0;
};

using Utf8CodePointIterator = CodePointIterator<char, &Utf8NextCodePoint>;
using Utf16CodePointIterator =
    CodePointIterator<Utf16CodeUnit, &Utf16NextCodePoint>;

Index CRU_BASE_API Utf8IndexCodeUnitToCodePoint(const Utf8CodeUnit* ptr,
                                                Index size, Index position);

inline Index Utf8IndexCodeUnitToCodePoint(Utf8StringView str, Index position) {
  return Utf8IndexCodeUnitToCodePoint(str.data(), str.size(), position);
}

Index CRU_BASE_API Utf8IndexCodePointToCodeUnit(const Utf8CodeUnit* ptr,
                                                Index size, Index position);

inline Index Utf8IndexCodePointToCodeUnit(Utf8StringView str, Index position) {
  return Utf8IndexCodePointToCodeUnit(str.data(), str.size(), position);
}

Index CRU_BASE_API Utf16IndexCodeUnitToCodePoint(const Utf16CodeUnit* ptr,
                                                 Index size, Index position);

inline Index Utf16IndexCodeUnitToCodePoint(Utf16StringView str,
                                           Index position) {
  return Utf16IndexCodeUnitToCodePoint(str.data(), str.size(), position);
}

#ifdef _WIN32
inline Index Utf16IndexCodeUnitToCodePoint(const wchar_t* ptr, Index size,
                                           Index position) {
  return Utf16IndexCodeUnitToCodePoint(reinterpret_cast<const char16_t*>(ptr),
                                       size, position);
}

inline Index Utf16IndexCodeUnitToCodePoint(std::wstring_view str,
                                           Index position) {
  return Utf16IndexCodeUnitToCodePoint(str.data(), str.size(), position);
}
#endif

Index CRU_BASE_API Utf16IndexCodePointToCodeUnit(const Utf16CodeUnit* ptr,
                                                 Index size, Index position);

inline Index Utf16IndexCodePointToCodeUnit(Utf16StringView str,
                                           Index position) {
  return Utf16IndexCodePointToCodeUnit(str.data(), str.size(), position);
}

#ifdef _WIN32
inline Index Utf16IndexCodePointToCodeUnit(const wchar_t* ptr, Index size,
                                           Index position) {
  return Utf16IndexCodePointToCodeUnit(reinterpret_cast<const char16_t*>(ptr),
                                       size, position);
}

inline Index Utf16IndexCodePointToCodeUnit(std::wstring_view str,
                                           Index position) {
  return Utf16IndexCodePointToCodeUnit(str.data(), str.size(), position);
}
#endif

#ifdef _WIN32
std::wstring CRU_BASE_API ToUtf16WString(std::string_view str);
std::string CRU_BASE_API ToUtf8String(std::wstring_view str);
#endif
}  // namespace cru::string
