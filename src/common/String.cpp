#include "cru/common/String.hpp"
#include "cru/common/Exception.hpp"
#include "cru/common/StringUtil.hpp"

#include <gsl/gsl>

#include <algorithm>
#include <cstring>
#include <functional>
#include <string_view>

namespace cru {
template <typename C>
Index GetStrSize(const C* str) {
  Index i = 0;
  while (str[i]) {
    i++;
  }
  return i;
}

String String::FromUtf8(const char* str) {
  return FromUtf16(cru::ToUtf16(str, GetStrSize(str)));
}

String String::FromUtf8(const char* str, Index size) {
  return FromUtf16(cru::ToUtf16(str, size));
}

char16_t String::kEmptyBuffer[1] = {0};

String::String(const_pointer str) : String(str, GetStrSize(str)) {}

String::String(const_pointer str, Index size) {
  this->buffer_ = new value_type[size + 1];
  std::memcpy(this->buffer_, str, size * sizeof(char16_t));
  this->buffer_[size] = 0;
  this->size_ = size;
  this->capacity_ = size;
}

String::String(std::initializer_list<char16_t> l)
    : String(l.begin(), l.size()) {}

#ifdef CRU_PLATFORM_WINDOWS
String::String(const wchar_t* str) : String(str, GetStrSize(str)) {}
String::String(const wchar_t* str, Index size)
    : String(reinterpret_cast<const std::uint16_t*>(str), size) {}
#endif

String::String(const String& other) {
  if (other.size_ == 0) return;
  this->buffer_ = new value_type[other.size_ + 1];
  std::memcpy(this->buffer_, other.buffer_, other.size_ * sizeof(value_type));
  this->buffer_[other.size_] = 0;
  this->size_ = other.size_;
  this->capacity_ = other.size_;
}

String::String(String&& other) noexcept {
  this->buffer_ = other.buffer_;
  this->size_ = other.size_;
  this->capacity_ = other.capacity_;
  other.buffer_ = kEmptyBuffer;
  other.size_ = 0;
  other.capacity_ = 0;
}

String& String::operator=(const String& other) {
  if (this != &other) {
    if (this->buffer_ != kEmptyBuffer) {
      delete[] this->buffer_;
    }

    if (other.buffer_ == kEmptyBuffer) {
      this->buffer_ = kEmptyBuffer;
      this->size_ = 0;
      this->capacity_ = 0;
    } else {
      this->buffer_ = new value_type[other.size_ + 1];
      std::memcpy(this->buffer_, other.buffer_,
                  other.size_ * sizeof(value_type));
      this->buffer_[other.size_] = 0;
      this->size_ = other.size_;
      this->capacity_ = other.size_;
    }
  }
  return *this;
}

String& String::operator=(String&& other) noexcept {
  if (this != &other) {
    if (this->buffer_ != kEmptyBuffer) {
      delete[] this->buffer_;
    }

    this->buffer_ = other.buffer_;
    this->size_ = other.size_;
    this->capacity_ = other.capacity_;
    other.buffer_ = kEmptyBuffer;
    other.size_ = 0;
    other.capacity_ = 0;
  }
  return *this;
}

String::~String() {
  if (this->buffer_ != kEmptyBuffer) {
    delete[] this->buffer_;
  }
}

String::String(from_buffer_tag, pointer buffer, Index size, Index capacity)
    : buffer_(buffer), size_(size), capacity_(capacity) {}

void String::resize(Index new_size) {
  Expects(new_size >= 0);

  if (new_size == size_) return;

  if (new_size < size_) {
    size_ = new_size;
    buffer_[size_] = 0;
  } else {
    reserve(new_size);
    std::memset(buffer_ + size_, 0, sizeof(value_type) * (new_size - size_));
    buffer_[new_size] = 0;
    size_ = new_size;
  }
}

void String::shrink_to_fit() {
  if (capacity_ == size_) return;
  if (size_ == 0) {
    delete[] buffer_;
    buffer_ = kEmptyBuffer;
    size_ = 0;
    capacity_ = 0;
  } else {
    auto new_buffer = new value_type[size_ + 1];
    std::memcpy(new_buffer, buffer_, sizeof(value_type) * size_);
    delete[] buffer_;
    buffer_ = new_buffer;
    capacity_ = size_;
  }
}

void String::reserve(Index new_capacity) {
  Expects(new_capacity >= 0);
  if (new_capacity <= this->capacity_) return;
  if (new_capacity > 0) {
    pointer new_buffer = new value_type[new_capacity + 1];
    if (this->buffer_ != kEmptyBuffer) {
      memcpy(new_buffer, this->buffer_, this->size_ * sizeof(value_type));
      delete[] this->buffer_;
    }
    new_buffer[this->size_] = 0;
    this->buffer_ = new_buffer;
    this->capacity_ = new_capacity;
  }
}

String::iterator String::insert(const_iterator pos, const_iterator str,
                                Index size) {
  Expects(pos >= cbegin() && pos <= cend());

  std::vector<value_type> backup_buffer;
  if (str >= buffer_ && str < buffer_ + size_) {
    backup_buffer.resize(size);
    std::copy(str, str + size, backup_buffer.begin());
    str = backup_buffer.data();
  }

  Index index = pos - cbegin();
  Index new_size = size_ + size;
  if (new_size > capacity_) {
    auto new_capacity = capacity_;
    if (new_capacity == 0) {
      new_capacity = new_size;
    } else {
      while (new_capacity < new_size) {
        new_capacity *= 2;
      }
    }

    this->reserve(new_capacity);
  }

  std::memmove(begin() + index + size, begin() + index,
               (size_ - index) * sizeof(value_type));
  std::memcpy(begin() + index, str, size * sizeof(value_type));

  buffer_[new_size] = 0;
  size_ = new_size;

  return begin() + new_size;
}

String::iterator String::erase(const_iterator start, const_iterator end) {
  Expects(buffer_ <= start && start <= end && end <= buffer_ + size_);

  Index new_size = size_ - (end - start);

  auto s = const_cast<iterator>(start);
  auto e = const_cast<iterator>(end);

  std::memmove(e, s, (cend() - end) * sizeof(value_type));
  this->size_ = new_size;
  this->buffer_[new_size] = 0;

  return s;
}

std::string String::ToUtf8() const { return cru::ToUtf8(buffer_, size_); }

void String::AppendCodePoint(CodePoint code_point) {
  if (!Utf16EncodeCodePointAppendWithFunc(
          code_point, [this](char16_t c) { this->push_back(c); })) {
    throw TextEncodeException(u"Code point out of range.");
  }
}

Index String::IndexFromCodeUnitToCodePoint(Index code_unit_index) const {
  auto iter = CodePointIterator();
  Index result = 0;
  while (iter.GetPosition() < code_unit_index && !iter.IsPastEnd()) {
    ++iter;
    ++result;
  }
  return result;
}

Index String::IndexFromCodePointToCodeUnit(Index code_point_index) const {
  auto iter = CodePointIterator();
  Index cpi = 0;
  while (cpi < code_point_index && !iter.IsPastEnd()) {
    ++iter;
    ++cpi;
  }
  return iter.GetPosition();
}

Range String::RangeFromCodeUnitToCodePoint(Range code_unit_range) const {
  return Range::FromTwoSides(
      IndexFromCodeUnitToCodePoint(code_unit_range.GetStart()),
      IndexFromCodeUnitToCodePoint(code_unit_range.GetEnd()));
}

Range String::RangeFromCodePointToCodeUnit(Range code_point_range) const {
  return Range::FromTwoSides(
      IndexFromCodePointToCodeUnit(code_point_range.GetStart()),
      IndexFromCodePointToCodeUnit(code_point_range.GetEnd()));
}

String& String::operator+=(StringView other) {
  append(other);
  return *this;
}

namespace {
inline int Compare(char16_t left, char16_t right) {
  if (left < right) return -1;
  if (left > right) return 1;
  return 0;
}
}  // namespace

int String::Compare(const String& other) const {
  const_iterator i1 = cbegin();
  const_iterator i2 = other.cbegin();

  const_iterator end1 = cend();
  const_iterator end2 = other.cend();

  while (i1 != end1 && i2 != end2) {
    int r = cru::Compare(*i1, *i2);
    if (r != 0) return r;
    i1++;
    i2++;
  }

  if (i1 == end1) {
    if (i2 == end2) {
      return 0;
    } else {
      return -1;
    }
  } else {
    return 1;
  }
}

namespace details {
std::vector<FormatToken> ParseToFormatTokenList(const String& str) {
  std::vector<FormatToken> result;

  auto push_char = [&result](char16_t c) {
    if (result.empty() || result.back().type == FormatTokenType::PlaceHolder) {
      result.push_back(FormatToken{FormatTokenType::Text, String{}});
    }
    result.back().data.append(c);
  };

  bool last_is_left_bracket = false;
  for (auto c : str) {
    if (c == u'{') {
      if (last_is_left_bracket) {
        push_char(u'{');
        last_is_left_bracket = false;
      } else {
        last_is_left_bracket = true;
      }
    } else if (c == u'}') {
      if (last_is_left_bracket) {
        result.push_back(FormatToken{FormatTokenType::PlaceHolder, String{}});
      }
      last_is_left_bracket = false;
    } else {
      if (last_is_left_bracket) {
        push_char(u'{');
      }
      push_char(c);
      last_is_left_bracket = false;
    }
  }
  return result;
}

void FormatAppendFromFormatTokenList(
    String& current, const std::vector<FormatToken>& format_token_list,
    Index index) {
  for (Index i = index; i < format_token_list.size(); i++) {
    const auto& token = format_token_list[i];
    if (token.type == FormatTokenType::PlaceHolder) {
      current += u"{}";
    } else {
      current += token.data;
    }
  }
}
}  // namespace details

int StringView::Compare(const StringView& other) const {
  const_iterator i1 = cbegin();
  const_iterator i2 = other.cbegin();

  const_iterator end1 = cend();
  const_iterator end2 = other.cend();

  while (i1 != end1 && i2 != end2) {
    int r = cru::Compare(*i1, *i2);
    if (r != 0) return r;
    i1++;
    i2++;
  }

  if (i1 == end1) {
    if (i2 == end2) {
      return 0;
    } else {
      return -1;
    }
  } else {
    return 1;
  }
}

StringView StringView::substr(Index pos) {
  Expects(pos >= 0 && pos < size_);
  return StringView(ptr_ + pos, size_ - pos);
}

StringView StringView::substr(Index pos, Index size) {
  Expects(pos >= 0 && pos < size_);

  return StringView(ptr_ + pos, std::min(size, size_ - pos));
}

std::string StringView::ToUtf8() const { return cru::ToUtf8(ptr_, size_); }

String ToLower(StringView s) {
  String result;
  for (auto c : s) result.push_back(ToLower(c));
  return result;
}

String ToUpper(StringView s) {
  String result;
  for (auto c : s) result.push_back(ToUpper(c));
  return result;
}
}  // namespace cru
