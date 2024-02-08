#include "cru/common/String.h"

#include "cru/common/Exception.h"
#include "cru/common/StringToNumberConverter.h"
#include "cru/common/StringUtil.h"

#include <double-conversion/double-conversion.h>
#include <double-conversion/string-to-double.h>

#include <algorithm>
#include <cmath>
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
  return FromUtf8(str, GetStrSize(str));
}

String String::FromUtf8(const char* str, Index size) {
  String result;
  Utf8CodePointIterator iter(str, size);
  for (auto cp : iter) {
    Utf16EncodeCodePointAppend(cp, result);
  }
  return result;
}

String String::FromStdPath(const std::filesystem::path& path) {
  return String::FromUtf8(path.string());
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

String::String(size_type size, value_type ch) : String() {
  reserve(size);
  for (Index i = 0; i < size; i++) {
    append(ch);
  }
}

String::String(std::initializer_list<char16_t> l)
    : String(l.begin(), l.size()) {}

#ifdef CRU_PLATFORM_WINDOWS
String::String(const wchar_t* str) : String(str, GetStrSize(str)) {}
String::String(const wchar_t* str, Index size)
    : String(reinterpret_cast<const char16_t*>(str), size) {}
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

void String::clear() { resize(0); }

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

  std::memmove(s, e, (cend() - end) * sizeof(value_type));
  this->size_ = new_size;
  this->buffer_[new_size] = 0;

  return s;
}

String& String::operator+=(StringView other) {
  append(other);
  return *this;
}

StringView String::View() const { return *this; }

Index String::Find(value_type value, Index start) const {
  return View().Find(value, start);
}

std::vector<String> String::Split(value_type separator,
                                  bool remove_space_line) const {
  return View().Split(separator, remove_space_line);
}

std::vector<String> String::SplitToLines(bool remove_space_line) const {
  return View().SplitToLines(remove_space_line);
}

bool String::StartWith(StringView str) const { return View().StartWith(str); }

bool String::EndWith(StringView str) const { return View().EndWith(str); }

std::string String::ToUtf8() const { return View().ToUtf8(); }

String& String::TrimStart() {
  if (size_ == 0) return *this;

  auto start = begin();
  while (start != end() && IsWhitespace(*start)) {
    ++start;
  }

  if (start == end()) {
    clear();
  } else {
    erase(begin(), start);
  }

  return *this;
}

String& String::TrimEnd() {
  if (size_ == 0) return *this;
  while (size_ > 0 && IsWhitespace(buffer_[size_ - 1])) {
    size_--;
  }

  return *this;
}

String& String::Trim() {
  TrimStart();
  TrimEnd();
  return *this;
}

void String::AppendCodePoint(CodePoint code_point) {
  if (!Utf16EncodeCodePointAppend(code_point, *this)) {
    throw TextEncodeException(u"Code point out of range.");
  }
}

Index String::IndexFromCodeUnitToCodePoint(Index code_unit_index) const {
  return View().IndexFromCodeUnitToCodePoint(code_unit_index);
}

Index String::IndexFromCodePointToCodeUnit(Index code_point_index) const {
  return View().IndexFromCodePointToCodeUnit(code_point_index);
}

Range String::RangeFromCodeUnitToCodePoint(Range code_unit_range) const {
  return View().RangeFromCodeUnitToCodePoint(code_unit_range);
}

Range String::RangeFromCodePointToCodeUnit(Range code_point_range) const {
  return View().RangeFromCodePointToCodeUnit(code_point_range);
}

int String::ParseToInt(Index* processed_characters_count, unsigned flags,
                       int base) const {
  return View().ParseToInt(processed_characters_count, flags, base);
}

long long String::ParseToLongLong(Index* processed_characters_count,
                                  unsigned flags, int base) const {
  return View().ParseToLongLong(processed_characters_count, flags, base);
}

float String::ParseToFloat(Index* processed_characters_count,
                           unsigned flags) const {
  return View().ParseToFloat(processed_characters_count, flags);
}

double String::ParseToDouble(Index* processed_characters_count,
                             unsigned flags) const {
  return View().ParseToDouble(processed_characters_count, flags);
}

std::vector<float> String::ParseToFloatList(value_type separator) const {
  return View().ParseToFloatList(separator);
}

std::vector<double> String::ParseToDoubleList(value_type separator) const {
  return View().ParseToDoubleList(separator);
}

namespace {
inline int Compare(char16_t left, char16_t right) {
  if (left < right) return -1;
  if (left > right) return 1;
  return 0;
}

inline int CaseInsensitiveCompare(char16_t left, char16_t right) {
  return Compare(ToLower(left), ToLower(right));
}
}  // namespace

int String::Compare(const String& other) const { return View().Compare(other); }
int String::CaseInsensitiveCompare(const String& other) const {
  return View().CaseInsensitiveCompare(other);
}

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

int StringView::CaseInsensitiveCompare(const StringView& other) const {
  const_iterator i1 = cbegin();
  const_iterator i2 = other.cbegin();

  const_iterator end1 = cend();
  const_iterator end2 = other.cend();

  while (i1 != end1 && i2 != end2) {
    int r = cru::CaseInsensitiveCompare(*i1, *i2);
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

Index StringView::Find(value_type value, Index start) const {
  Expects(start >= 0 && start <= size_);

  for (Index i = start; i < size_; ++i) {
    if (ptr_[i] == value) return i;
  }
  return -1;
}

std::vector<String> StringView::Split(value_type separator,
                                      bool remove_space_line) const {
  std::vector<String> result;

  if (size_ == 0) return result;

  Index line_start = 0;
  Index line_end = 0;
  while (line_end < size_) {
    if (ptr_[line_end] == separator) {
      if (remove_space_line) {
        bool add = false;
        for (Index i = line_start; i < line_end; i++) {
          if (!IsWhitespace(ptr_[i])) {
            add = true;
            break;
          }
        }
        if (add) result.emplace_back(begin() + line_start, begin() + line_end);
      } else {
        result.emplace_back(begin() + line_start, begin() + line_end);
      }
      line_start = line_end + 1;
      line_end = line_start;
    } else {
      line_end++;
    }
  }

  if (remove_space_line) {
    bool add = false;
    for (Index i = line_start; i < size_; i++) {
      if (!IsWhitespace(ptr_[i])) {
        add = true;
        break;
      }
    }
    if (add) result.emplace_back(begin() + line_start, begin() + size_);
  } else {
    result.emplace_back(begin() + line_start, begin() + size_);
  }

  return result;
}

std::vector<String> StringView::SplitToLines(bool remove_space_line) const {
  return Split(u'\n', remove_space_line);
}

bool StringView::StartWith(StringView str) const {
  if (str.size() > size_) return false;
  return std::memcmp(str.data(), ptr_, str.size()) == 0;
}

bool StringView::EndWith(StringView str) const {
  if (str.size() > size_) return false;
  return std::memcmp(str.data(), ptr_ + size_ - str.size(), str.size()) == 0;
}

Index StringView::IndexFromCodeUnitToCodePoint(Index code_unit_index) const {
  auto iter = CodePointIterator();
  Index result = 0;
  while (iter.GetPosition() < code_unit_index && !iter.IsPastEnd()) {
    ++iter;
    ++result;
  }
  return result;
}

Index StringView::IndexFromCodePointToCodeUnit(Index code_point_index) const {
  auto iter = CodePointIterator();
  Index cpi = 0;
  while (cpi < code_point_index && !iter.IsPastEnd()) {
    ++iter;
    ++cpi;
  }
  return iter.GetPosition();
}

Range StringView::RangeFromCodeUnitToCodePoint(Range code_unit_range) const {
  return Range::FromTwoSides(
      IndexFromCodeUnitToCodePoint(code_unit_range.GetStart()),
      IndexFromCodeUnitToCodePoint(code_unit_range.GetEnd()));
}

Range StringView::RangeFromCodePointToCodeUnit(Range code_point_range) const {
  return Range::FromTwoSides(
      IndexFromCodePointToCodeUnit(code_point_range.GetStart()),
      IndexFromCodePointToCodeUnit(code_point_range.GetEnd()));
}

std::string StringView::ToUtf8() const {
  std::string result;
  for (auto cp : CodePointIterator()) {
    Utf8EncodeCodePointAppend(cp, result);
  }
  return result;
}

int StringView::ParseToInt(Index* processed_characters_count, unsigned flags,
                           int base) const {
  return ParseToInteger<int>(processed_characters_count, flags, base);
}

long long StringView::ParseToLongLong(Index* processed_characters_count,
                                      unsigned flags, int base) const {
  return ParseToInteger<long long>(processed_characters_count, flags, base);
}

static int MapStringToDoubleFlags(int flags) {
  int f = double_conversion::StringToDoubleConverter::ALLOW_CASE_INSENSIBILITY;
  if (flags & StringToNumberFlags::kAllowLeadingSpaces) {
    f |= double_conversion::StringToDoubleConverter::ALLOW_LEADING_SPACES;
  }
  if (flags & StringToNumberFlags::kAllowTrailingSpaces) {
    f |= double_conversion::StringToDoubleConverter::ALLOW_TRAILING_SPACES;
  }
  if (flags & StringToNumberFlags::kAllowTrailingJunk) {
    f |= double_conversion::StringToDoubleConverter::ALLOW_TRAILING_JUNK;
  }
  return f;
}

static double_conversion::StringToDoubleConverter CreateStringToDoubleConverter(
    int flags) {
  return {MapStringToDoubleFlags(flags), 0.0, NAN, "inf", "nan"};
}

float StringView::ParseToFloat(Index* processed_characters_count,
                               unsigned flags) const {
  int pcc;
  auto result = CreateStringToDoubleConverter(flags).StringToFloat(
      reinterpret_cast<const uc16*>(ptr_), static_cast<int>(size_), &pcc);
  if (processed_characters_count != nullptr) {
    *processed_characters_count = pcc;
  }

  if (flags & StringToNumberFlags::kThrowOnError && std::isnan(result)) {
    throw Exception(u"Result of string to float conversion is NaN");
  }

  return result;
}

double StringView::ParseToDouble(Index* processed_characters_count,
                                 unsigned flags) const {
  int pcc;
  auto result = CreateStringToDoubleConverter(flags).StringToDouble(
      reinterpret_cast<const uc16*>(ptr_), static_cast<int>(size_), &pcc);
  if (processed_characters_count != nullptr) {
    *processed_characters_count = pcc;
  }

  if (flags & StringToNumberFlags::kThrowOnError && std::isnan(result)) {
    throw Exception(u"Result of string to double conversion is NaN");
  }

  return result;
}

std::vector<float> StringView::ParseToFloatList(value_type separator) const {
  std::vector<float> result;
  auto list = Split(separator, true);
  for (auto& item : list) {
    auto value = item.ParseToFloat();
    if (std::isnan(value)) {
      throw Exception(u"Invalid double value.");
    }
    result.push_back(value);
  }
  return result;
}

std::vector<double> StringView::ParseToDoubleList(value_type separator) const {
  std::vector<double> result;
  auto list = Split(separator, true);
  for (auto& item : list) {
    auto value = item.ParseToDouble();
    if (std::isnan(value)) {
      throw Exception(u"Invalid double value.");
    }
    result.push_back(value);
  }
  return result;
}

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
