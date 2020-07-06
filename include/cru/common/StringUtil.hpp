#pragma once
#include "Base.hpp"

namespace cru {
using CodePoint = std::int32_t;
constexpr CodePoint k_invalid_code_point = -1;

class TextEncodeException : public std::runtime_error {
 public:
  using runtime_error::runtime_error;
};

inline bool IsUtf16SurrogatePairCodeUnit(char16_t c) {
  return c >= 0xD800 && c <= 0xDFFF;
}

inline bool IsUtf16SurrogatePairLeading(char16_t c) {
  return c >= 0xD800 && c <= 0xDBFF;
}

inline bool IsUtf16SurrogatePairTrailing(char16_t c) {
  return c >= 0xDC00 && c <= 0xDFFF;
}

CodePoint Utf8NextCodePoint(std::string_view str, Index current,
                            Index* next_position);

CodePoint Utf16NextCodePoint(std::u16string_view str, Index current,
                             Index* next_position);
CodePoint Utf16PreviousCodePoint(std::u16string_view str, Index current,
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
  mutable Index next_position_cache_;
};

using Utf8CodePointIterator =
    CodePointIterator<std::string_view, &Utf8NextCodePoint>;

using Utf16CodePointIterator =
    CodePointIterator<std::u16string_view, &Utf16NextCodePoint>;

std::string ToUtf8(const std::u16string& s);
inline std::string ToUtf8(std::u16string_view s) {
  return ToUtf8(std::u16string{s});
}

// class Utf8Iterator : public Object {
//  public:
//   explicit Utf8Iterator(const std::string_view& string) : string_(string) {}
//   Utf8Iterator(const std::string_view& string, Index position)
//       : string_(string), position_(position) {}

//   CRU_DEFAULT_COPY(Utf8Iterator)
//   CRU_DEFAULT_MOVE(Utf8Iterator)

//   ~Utf8Iterator() = default;

//  public:
//   void SetToHead() { position_ = 0; }
//   void SetPosition(Index position) { position_ = position; }

//   // Advance current position and get next code point. Return
//   k_invalid_code_point
//   // if there is no next code unit(point). Throw TextEncodeException if
//   decoding
//   // fails.
//   CodePoint Next();

//   Index CurrentPosition() const { return this->position_; }

//  private:
//   std::string_view string_;
//   Index position_ = 0;
// };
}  // namespace cru
