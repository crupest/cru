#pragma once
#include "Base.h"
#include "Bitmask.h"

#include <functional>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace cru {
namespace details {
struct SplitOptionsTag {};
}  // namespace details
using SplitOption = Bitmask<details::SplitOptionsTag>;
struct SplitOptions {
  static constexpr SplitOption RemoveEmpty = SplitOption::FromOffset(1);
};

template <typename TString>
std::vector<TString> Split(const TString& str, const TString& sep,
                           SplitOption options = {}) {
  using size_type = typename TString::size_type;

  if (sep.empty()) throw std::invalid_argument("Sep can't be empty.");
  if (str.empty()) return {};

  size_type current_pos = 0;
  std::vector<TString> result;

  while (current_pos != TString::npos) {
    if (current_pos == str.size()) {
      if (!options.Has(SplitOptions::RemoveEmpty)) {
        result.push_back({});
      }
      break;
    }

    auto next_pos = str.find(sep, current_pos);
    auto sub = str.substr(current_pos, next_pos - current_pos);
    if (!(options.Has(SplitOptions::RemoveEmpty) && sub.empty())) {
      result.push_back(sub);
    }
    current_pos =
        next_pos == TString::npos ? TString::npos : next_pos + sep.size();
  }

  return result;
}

inline std::vector<std::string> Split(const char* str, const std::string& sep,
                                      SplitOption options = {}) {
  return Split<std::string>(std::string(str), sep, options);
}

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

}  // namespace cru
