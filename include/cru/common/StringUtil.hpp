#pragma once
#include "Base.hpp"

#include <functional>
#include <string>
#include <string_view>

namespace cru {
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

CodePoint CRU_BASE_API Utf8NextCodePoint(std::string_view str, Index current,
                                         Index* next_position);

CodePoint CRU_BASE_API Utf16NextCodePoint(std::u16string_view str,
                                          Index current, Index* next_position);
CodePoint CRU_BASE_API Utf16PreviousCodePoint(std::u16string_view str,
                                              Index current,
                                              Index* previous_position);

template <typename StringType>
using NextCodePointFunctionType = CodePoint (*)(StringType, Index, Index*);

template <typename StringType,
          NextCodePointFunctionType<StringType> NextCodePointFunction>
class CodePointIterator {
 public:
  using difference_type = Index;
  using value_type = CodePoint;
  using pointer = void;
  using reference = value_type;
  using iterator_category = std::forward_iterator_tag;

 public:
  struct past_end_tag_t {};

  explicit CodePointIterator(StringType string)
      : string_(std::move(string)), position_(0) {}
  explicit CodePointIterator(StringType string, past_end_tag_t)
      : string_(std::move(string)), position_(string_.size()) {}

  CRU_DEFAULT_COPY(CodePointIterator)
  CRU_DEFAULT_MOVE(CodePointIterator)

  ~CodePointIterator() = default;

 public:
  StringType GetString() const { return string_; }
  Index GetPosition() const { return position_; }

  bool IsPastEnd() const {
    return position_ == static_cast<Index>(string_.size());
  }

 public:
  CodePointIterator begin() const { return *this; }
  CodePointIterator end() const {
    return CodePointIterator{string_, past_end_tag_t{}};
  }

 public:
  bool operator==(const CodePointIterator& other) const {
    // You should compare iterator that iterate on the same string.
    Expects(this->string_.data() == other.string_.data() &&
            this->string_.size() == other.string_.size());
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
    return NextCodePointFunction(string_, position_, &next_position_cache_);
  }

 private:
  void Forward() {
    if (next_position_cache_ > position_) {
      position_ = next_position_cache_;
    } else {
      NextCodePointFunction(string_, position_, &position_);
    }
  }

 private:
  StringType string_;
  Index position_;
  mutable Index next_position_cache_ = 0;
};

using Utf8CodePointIterator =
    CodePointIterator<std::string_view, &Utf8NextCodePoint>;

using Utf16CodePointIterator =
    CodePointIterator<std::u16string_view, &Utf16NextCodePoint>;

void CRU_BASE_API Utf8EncodeCodePointAppend(CodePoint code_point,
                                            std::string& str);
void CRU_BASE_API Utf16EncodeCodePointAppend(CodePoint code_point,
                                             std::u16string& str);

std::string CRU_BASE_API ToUtf8(std::u16string_view s);
std::u16string CRU_BASE_API ToUtf16(std::string_view s);

// If given s is not a valid utf16 string, return value is UD.
bool CRU_BASE_API Utf16IsValidInsertPosition(std::u16string_view s,
                                             gsl::index position);

// Return position after the character making predicate returns true or 0 if no
// character doing so.
gsl::index CRU_BASE_API
Utf16BackwardUntil(std::u16string_view str, gsl::index position,
                   const std::function<bool(CodePoint)>& predicate);
// Return position before the character making predicate returns true or
// str.size() if no character doing so.
gsl::index CRU_BASE_API
Utf16ForwardUntil(std::u16string_view str, gsl::index position,
                  const std::function<bool(CodePoint)>& predicate);

gsl::index CRU_BASE_API Utf16PreviousWord(std::u16string_view str,
                                          gsl::index position,
                                          bool* is_space = nullptr);
gsl::index CRU_BASE_API Utf16NextWord(std::u16string_view str,
                                      gsl::index position,
                                      bool* is_space = nullptr);

char16_t CRU_BASE_API ToLower(char16_t c);
char16_t CRU_BASE_API ToUpper(char16_t c);
std::u16string CRU_BASE_API ToLower(std::u16string_view s);
std::u16string CRU_BASE_API ToUpper(std::u16string_view s);
}  // namespace cru
