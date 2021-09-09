#include "cru/common/String.hpp"
#include "cru/common/StringUtil.hpp"

#include <cstring>
#include <string_view>

namespace cru {
String String::FromUtf8(const char* str, Index size) {
  return String{cru::ToUtf16(std::string_view(str, size))};
}

std::uint16_t String::kEmptyBuffer[1] = {0};

template <typename C>
Index GetStrSize(const C* str) {
  Index i = 0;
  while (str[i]) {
    i++;
  }
  return i;
}

String::String(const std::uint16_t* str) : String(str, GetStrSize(str)) {}
String::String(const std::uint16_t* str, Index size) {
  this->buffer_ = new std::uint16_t[size + 1];
  std::memcpy(this->buffer_, str, size * sizeof(char16_t));
  this->buffer_[size] = 0;
  this->size_ = size;
  this->capacity_ = size;
}

String::String(const char16_t* str) : String(str, GetStrSize(str)) {}
String::String(const char16_t* str, Index size)
    : String(reinterpret_cast<const std::uint16_t*>(str), size) {}

#ifdef CRU_PLATFORM_WINDOWS
String::String(const wchar_t* str) : String(str, GetStrSize(str)) {}
String::String(const wchar_t* str, Index size)
    : String(reinterpret_cast<const std::uint16_t*>(str), size) {}
#endif

String::String(const String& other) {
  if (other.buffer_ == kEmptyBuffer) return;
  this->buffer_ = new std::uint16_t[other.size_ + 1];
  std::memcpy(this->buffer_, other.buffer_, other.size_ * sizeof(wchar_t));
  this->buffer_[other.size_] = 0;
  this->size_ = other.size_;
  this->capacity_ = other.size_;
}

String::String(String&& other) noexcept {
  if (other.buffer_ == kEmptyBuffer) return;
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
      this->buffer_ = new std::uint16_t[other.size_ + 1];
      std::memcpy(this->buffer_, other.buffer_, other.size_ * sizeof(wchar_t));
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

void String::reserve(Index new_capacity) {
  if (new_capacity <= this->capacity_) return;
  if (new_capacity > 0) {
    std::uint16_t* new_buffer = new std::uint16_t[new_capacity + 1];
    if (this->buffer_ != kEmptyBuffer) {
      memcpy(new_buffer, this->buffer_, this->size_ * sizeof(std::uint16_t));
      delete[] this->buffer_;
    }
    new_buffer[this->size_] = 0;
    this->capacity_ = new_capacity;
  }
}

String::iterator String::insert(const_iterator pos, std::uint16_t* str,
                                Index size) {
  Index new_size = size_ + size;
  if (new_size > capacity_) {
    this->reserve(this->capacity_ * 2);
  }

  auto p = const_cast<iterator>(pos);

  std::memmove(p + size, pos, (cend() - pos) * sizeof(std::uint16_t));
  std::memcpy(p, str, size * sizeof(std::uint16_t));

  buffer_[new_size] = 0;

  return p + size;
}

String::iterator String::erase(const_iterator start, const_iterator end) {
  Index new_size = size_ - (end - start);

  auto s = const_cast<iterator>(start);
  auto e = const_cast<iterator>(end);

  std::memmove(e, s, (cend() - end) * sizeof(std::uint16_t));
  this->size_ = new_size;
  this->buffer_[new_size] = 0;

  return s;
}

std::string String::ToUtf8() const {
  return cru::ToUtf8(std::u16string_view(Char16CStr(), size()));
}

Index String::IndexFromCodeUnitToCodePoint(Index code_unit_index) const {
  auto iter = CodePointIterator();
  Index result = 0;
  while (iter.GetPosition() <= code_unit_index && !iter.IsPastEnd()) {
    ++iter;
    ++result;
  }
  return result - 1;
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

namespace {
inline int Compare(std::uint16_t left, std::uint16_t right) {
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
  }

  if (i1 == end1) {
    if (i2 == end1) {
      return 0;
    } else {
      return -1;
    }
  } else {
    return 1;
  }
}

}  // namespace cru
