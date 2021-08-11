#include "cru/common/String.hpp"

#include <cstring>

namespace cru {
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

}  // namespace cru
