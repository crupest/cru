#pragma once
#include "Base.hpp"

#include "Range.hpp"
#include "StringUtil.hpp"

#include <cstdint>
#include <iterator>

namespace cru {
class CRU_BASE_API String {
 public:
  static String FromUtf8(std::string_view str) {
    return FromUtf8(str.data(), str.size());
  }
  static String FromUtf8(const char* str, Index size);

  static String FromUtf16(const std::uint16_t* str) { return String(str); }
  static String FromUtf16(const std::uint16_t* str, Index size) {
    return String(str, size);
  }

  static String FromUtf16(const char16_t* str) { return String(str); }
  static String FromUtf16(const char16_t* str, Index size) {
    return String(str, size);
  }

#ifdef CRU_PLATFORM_WINDOWS
  static String FromUtf16(wchar_t* str) { return String(str); }
  static String FromUtf16(wchar_t* str, Index size) {
    return String(str, size);
  }
#endif

 public:
  using value_type = std::uint16_t;
  using size_type = Index;
  using difference_type = Index;
  using reference = std::uint16_t&;
  using const_reference = const std::uint16_t&;
  using pointer = std::uint16_t*;
  using const_pointer = const std::uint16_t*;
  using iterator = std::uint16_t*;
  using const_iterator = const std::uint16_t*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

 public:
  String() = default;

  String(const std::uint16_t* str);
  String(const std::uint16_t* str, Index size);

  String(const char16_t* str);
  String(const char16_t* str, Index size);
  String(const std::u16string& str) : String(str.data(), str.size()) {}

#ifdef CRU_PLATFORM_WINDOWS
  String(const wchar_t* str);
  String(const wchar_t* str, Index size);
  String(const std::wstring& str) : String(str.data(), str.size()) {}
#endif

  String(const String& other);
  String(String&& other) noexcept;

  String& operator=(const String& other);
  String& operator=(String&& other) noexcept;

  ~String();

 public:
  bool empty() const { return this->size_ == 0; }
  Index size() const { return this->size_; }
  Index length() const { return this->size(); }
  Index capacity() const { return this->capacity_; }
  std::uint16_t* data() { return this->buffer_; }
  const std::uint16_t* data() const { return this->buffer_; }

  void resize(Index new_size);
  void reserve(Index new_capacity);

  std::uint16_t& front() { return this->operator[](0); }
  const std::uint16_t& front() const { return this->operator[](0); }

  std::uint16_t& back() { return this->operator[](size_ - 1); }
  const std::uint16_t& back() const { return this->operator[](size_ - 1); }

  const std::uint16_t* c_str() const { return buffer_; }

  std::uint16_t& operator[](Index index) { return buffer_[index]; }
  const std::uint16_t& operator[](Index index) const { return buffer_[index]; }

 public:
  iterator begin() { return this->buffer_; }
  const_iterator begin() const { return this->buffer_; }
  const_iterator cbegin() const { return this->buffer_; }

  iterator end() { return this->buffer_ + this->size_; }
  const_iterator end() const { return this->buffer_ + this->size_; }
  const_iterator cend() const { return this->buffer_ + this->size_; }

  reverse_iterator rbegin() { return reverse_iterator{begin()}; }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator{begin()};
  }
  const_reverse_iterator crbegin() const {
    return const_reverse_iterator{cbegin()};
  }

  reverse_iterator rend() { return reverse_iterator{end()}; }
  const_reverse_iterator rend() const { return const_reverse_iterator{end()}; }
  const_reverse_iterator crend() const {
    return const_reverse_iterator{cend()};
  }

 public:
  void clear();
  iterator insert(const_iterator pos, std::uint16_t value) {
    return this->insert(pos, &value, 1);
  }
  iterator insert(const_iterator pos, const std::uint16_t* str, Index size);
  iterator erase(const_iterator pos) { return this->erase(pos, pos + 1); }
  iterator erase(const_iterator start, const_iterator end);
  void push_back(std::uint16_t value) { this->append(value); }
  void pop_back() { this->erase(cend() - 1); }
  void append(std::uint16_t value) { this->insert(cend(), value); }
  void append(const std::uint16_t* str, Index size) {
    this->insert(cend(), str, size);
  }
  void append(const String& other) { append(other.data(), other.size()); }

 public:
  String& operator+=(const String& other) {
    append(other);
    return *this;
  }

  String operator+(const String& other) const {
    String result(*this);
    result += other;
    return result;
  }

 public:
  Utf16CodePointIterator CodePointIterator() const {
    return Utf16CodePointIterator(
        std::u16string_view(reinterpret_cast<char16_t*>(buffer_), size_));
  }

  Index IndexFromCodeUnitToCodePoint(Index code_unit_index) const;
  Index IndexFromCodePointToCodeUnit(Index code_point_index) const;
  Range RangeFromCodeUnitToCodePoint(Range code_unit_range) const;
  Range RangeFromCodePointToCodeUnit(Range code_point_range) const;

  const char16_t* Char16CStr() const {
    return reinterpret_cast<const char16_t*>(c_str());
  }

#ifdef CRU_PLATFORM_WINDOWS
  const wchar_t* WinCStr() const {
    return reinterpret_cast<const wchar_t*>(c_str());
  }
#endif

  std::string ToUtf8() const;

  int Compare(const String& other) const;

 private:
  static std::uint16_t kEmptyBuffer[1];

 private:
  std::uint16_t* buffer_ = kEmptyBuffer;
  Index size_ = 0;      // not including trailing '\0'
  Index capacity_ = 0;  // always 1 smaller than real buffer size
};

CRU_DEFINE_COMPARE_OPERATORS(String)

}  // namespace cru
