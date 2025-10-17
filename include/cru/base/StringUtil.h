#pragma once
#include "Base.h"
#include "Bitmask.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <compare>
#include <format>
#include <functional>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <vector>

namespace cru {
namespace string {
std::weak_ordering CaseInsensitiveCompare(std::string_view left,
                                          std::string_view right);
std::string TrimBegin(std::string_view str);
std::string TrimEnd(std::string_view str);
std::string Trim(std::string_view str);
bool IsSpace(std::string_view str);

namespace details {
struct SplitOptionsTag {};
}  // namespace details
using SplitOption = Bitmask<details::SplitOptionsTag>;
struct SplitOptions {
  static constexpr SplitOption RemoveEmpty = SplitOption::FromOffset(1);
  static constexpr SplitOption RemoveSpace = SplitOption::FromOffset(2);
};

std::vector<std::string> Split(std::string_view str, std::string_view sep,
                               SplitOption options = {});

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

  auto parse_result =
      std::from_chars(ptr, str.data() + str.size(), result.value);
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

}  // namespace string

using CodePoint = std::int32_t;
constexpr CodePoint k_invalid_code_point = -1;

inline bool IsUtf16SurrogatePairCodeUnit(char16_t c) {
  return c >= 0xD800 && c <= 0xDFFF;
}

inline bool IsUtf16SurrogatePairLeading(char16_t c) {
  return c >= 0xD800 && c <= 0xDBFF;
}

inline bool IsUtf16SurrogatePairTrailing(char16_t c) {
  return c >= 0xDC00 && c <= 0xDFFF;
}

CodePoint CRU_BASE_API Utf8NextCodePoint(const char* ptr, Index size,
                                         Index current, Index* next_position);

CodePoint CRU_BASE_API Utf16NextCodePoint(const char16_t* ptr, Index size,
                                          Index current, Index* next_position);
CodePoint CRU_BASE_API Utf16PreviousCodePoint(const char16_t* ptr, Index size,
                                              Index current,
                                              Index* previous_position);

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

using Utf16CodePointIterator = CodePointIterator<char16_t, &Utf16NextCodePoint>;

namespace details {
template <typename UInt, int number_of_bit, typename ReturnType>
inline std::enable_if_t<std::is_unsigned_v<UInt>, ReturnType> ExtractBits(
    UInt n) {
  return static_cast<ReturnType>(n & ((1u << number_of_bit) - 1));
}
}  // namespace details

template <typename CharWriter>
std::enable_if_t<std::is_invocable_v<CharWriter, char>, bool>
Utf8EncodeCodePointAppend(CodePoint code_point, CharWriter&& writer) {
  auto write_continue_byte = [&writer](std::uint8_t byte6) {
    writer((1u << 7) + (((1u << 6) - 1) & byte6));
  };

  if (code_point >= 0 && code_point <= 0x007F) {
    writer(static_cast<char>(code_point));
    return true;
  } else if (code_point >= 0x0080 && code_point <= 0x07FF) {
    std::uint32_t unsigned_code_point = code_point;
    writer(
        static_cast<char>(details::ExtractBits<std::uint32_t, 5, std::uint8_t>(
                              (unsigned_code_point >> 6)) +
                          0b11000000));
    write_continue_byte(details::ExtractBits<std::uint32_t, 6, std::uint8_t>(
        unsigned_code_point));
    return true;
  } else if (code_point >= 0x0800 && code_point <= 0xFFFF) {
    std::uint32_t unsigned_code_point = code_point;
    writer(
        static_cast<char>(details::ExtractBits<std::uint32_t, 4, std::uint8_t>(
                              (unsigned_code_point >> (6 * 2))) +
                          0b11100000));
    write_continue_byte(details::ExtractBits<std::uint32_t, 6, std::uint8_t>(
        unsigned_code_point >> 6));
    write_continue_byte(details::ExtractBits<std::uint32_t, 6, std::uint8_t>(
        unsigned_code_point));
    return true;
  } else if (code_point >= 0x10000 && code_point <= 0x10FFFF) {
    std::uint32_t unsigned_code_point = code_point;
    writer(
        static_cast<char>(details::ExtractBits<std::uint32_t, 3, std::uint8_t>(
                              (unsigned_code_point >> (6 * 3))) +
                          0b11110000));
    write_continue_byte(details::ExtractBits<std::uint32_t, 6, std::uint8_t>(
        unsigned_code_point >> (6 * 2)));
    write_continue_byte(details::ExtractBits<std::uint32_t, 6, std::uint8_t>(
        unsigned_code_point >> 6));
    write_continue_byte(details::ExtractBits<std::uint32_t, 6, std::uint8_t>(
        unsigned_code_point));
    return true;
  } else {
    return false;
  }
}

template <typename CharWriter>
std::enable_if_t<std::is_invocable_v<CharWriter, char16_t>, bool>
Utf16EncodeCodePointAppend(CodePoint code_point, CharWriter&& writer) {
  if ((code_point >= 0 && code_point <= 0xD7FF) ||
      (code_point >= 0xE000 && code_point <= 0xFFFF)) {
    writer(static_cast<char16_t>(code_point));
    return true;
  } else if (code_point >= 0x10000 && code_point <= 0x10FFFF) {
    std::uint32_t u = code_point - 0x10000;
    writer(static_cast<char16_t>(
        details::ExtractBits<std::uint32_t, 10, std::uint32_t>(u >> 10) +
        0xD800u));
    writer(static_cast<char16_t>(
        details::ExtractBits<std::uint32_t, 10, std::uint32_t>(u) + 0xDC00u));
    return true;
  } else {
    return false;
  }
}

// If given s is not a valid utf16 string, return value is UD.
bool CRU_BASE_API Utf16IsValidInsertPosition(const char16_t* ptr, Index size,
                                             Index position);

// Return position after the character making predicate returns true or 0 if no
// character doing so.
Index CRU_BASE_API
Utf16BackwardUntil(const char16_t* ptr, Index size, Index position,
                   const std::function<bool(CodePoint)>& predicate);
// Return position before the character making predicate returns true or
// str.size() if no character doing so.
Index CRU_BASE_API
Utf16ForwardUntil(const char16_t* ptr, Index size, Index position,
                  const std::function<bool(CodePoint)>& predicate);

Index CRU_BASE_API Utf16PreviousWord(const char16_t* ptr, Index size,
                                     Index position, bool* is_space = nullptr);
Index CRU_BASE_API Utf16NextWord(const char16_t* ptr, Index size,
                                 Index position, bool* is_space = nullptr);

char16_t CRU_BASE_API ToLower(char16_t c);
char16_t CRU_BASE_API ToUpper(char16_t c);

bool CRU_BASE_API IsWhitespace(char16_t c);
bool CRU_BASE_API IsDigit(char16_t c);

Utf8CodePointIterator CRU_BASE_API CreateUtf8Iterator(const std::byte* buffer,
                                                      Index size);
Utf8CodePointIterator CRU_BASE_API
CreateUtf8Iterator(const std::vector<std::byte>& buffer);

CodePoint CRU_BASE_API Utf8NextCodePoint(std::string_view str, Index current,
                                         Index* next_position);
CodePoint CRU_BASE_API Utf8PreviousCodePoint(std::string_view str,
                                             Index current,
                                             Index* next_position);
// Return position after the character making predicate returns true or 0 if no
// character doing so.
Index CRU_BASE_API
Utf8BackwardUntil(std::string_view str, Index position,
                  const std::function<bool(CodePoint)>& predicate);
// Return position before the character making predicate returns true or
// str.size() if no character doing so.
Index CRU_BASE_API
Utf8ForwardUntil(std::string_view str, Index position,
                 const std::function<bool(CodePoint)>& predicate);

bool CRU_BASE_API Utf8IsValidInsertPosition(std::string_view str,
                                            Index position);

Index CRU_BASE_API Utf8PreviousWord(std::string_view str, Index position,
                                    bool* is_space = nullptr);
Index CRU_BASE_API Utf8NextWord(std::string_view str, Index position,
                                bool* is_space = nullptr);

}  // namespace cru
