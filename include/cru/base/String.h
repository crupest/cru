#pragma once
#include "Base.h"

#include "Buffer.h"
#include "Range.h"
#include "StringUtil.h"

#include <filesystem>
#include <initializer_list>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

namespace cru {
class StringView;

class CRU_BASE_API String {
 public:
  using value_type = char16_t;
  using size_type = Index;
  using difference_type = Index;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = value_type*;
  using const_iterator = const value_type*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

 public:
  static String FromUtf8(const char* str);
  static String FromUtf8(const char* str, Index size);
  static String FromUtf8(const std::byte* str, Index size);
  static String FromUtf8(std::string_view str) {
    return FromUtf8(str.data(), str.size());
  }
  static String FromUtf8(const Buffer& buffer);

  static String FromUtf16(const char16_t* str) { return String(str); }
  static String FromUtf16(const char16_t* str, Index size) {
    return String(str, size);
  }
  static String FromUtf16(std::u16string_view str) {
    return FromUtf16(str.data(), str.size());
  }

  static inline String From(StringView str);

  // Never use this if you don't know what this mean!
  static String FromBuffer(pointer buffer, Index size, Index capacity) {
    return String{from_buffer_tag{}, buffer, size, capacity};
  }

  static String FromStdPath(const std::filesystem::path& path);

#ifdef CRU_PLATFORM_WINDOWS
  static String FromUtf16(wchar_t* str) { return String(str); }
  static String FromUtf16(wchar_t* str, Index size) {
    return String(str, size);
  }
#endif

 public:
  String();

  String(const_pointer str);
  String(const_pointer str, size_type size);

  template <Index size>
  String(const char16_t (&str)[size])
      : String(reinterpret_cast<const_pointer>(str), size - 1) {}

  template <typename Iter>
  String(Iter start, Iter end): String() {
    for (; start != end; start++) {
      append(*start);
    }
  }

  String(size_type size, value_type ch = 0);

  String(std::initializer_list<value_type> l);

  explicit String(StringView str);

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

 private:
  struct from_buffer_tag {};
  String(from_buffer_tag, pointer buffer, Index size, Index capacity);

 public:
  bool empty() const { return this->size_ == 0; }
  Index size() const { return this->size_; }
  Index length() const { return this->size(); }
  Index capacity() const { return this->capacity_; }
  pointer data() { return this->buffer_; }
  const_pointer data() const { return this->buffer_; }

  void resize(Index new_size);
  void reserve(Index new_capacity);
  void shrink_to_fit();

  reference front() { return this->operator[](0); }
  const_reference front() const { return this->operator[](0); }

  reference back() { return this->operator[](size_ - 1); }
  const_reference back() const { return this->operator[](size_ - 1); }

  const_pointer c_str() const { return buffer_; }

  reference operator[](Index index) { return buffer_[index]; }
  const_reference operator[](Index index) const { return buffer_[index]; }

 public:
  iterator begin() { return this->buffer_; }
  const_iterator begin() const { return this->buffer_; }
  const_iterator cbegin() const { return this->buffer_; }

  iterator end() { return this->buffer_ + this->size_; }
  const_iterator end() const { return this->buffer_ + this->size_; }
  const_iterator cend() const { return this->buffer_ + this->size_; }

  reverse_iterator rbegin() { return reverse_iterator{end()}; }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator{end()};
  }
  const_reverse_iterator crbegin() const {
    return const_reverse_iterator{cend()};
  }

  reverse_iterator rend() { return reverse_iterator{begin()}; }
  const_reverse_iterator rend() const {
    return const_reverse_iterator{begin()};
  }
  const_reverse_iterator crend() const {
    return const_reverse_iterator{cbegin()};
  }

 public:
  void clear();
  iterator insert(const_iterator pos, value_type value) {
    return this->insert(pos, &value, 1);
  }
  iterator insert(const_iterator pos, const_iterator str, Index size);
  iterator insert(const_iterator pos, StringView str);
  iterator erase(const_iterator pos) { return this->erase(pos, pos + 1); }
  iterator erase(const_iterator start, const_iterator end);
  void push_back(value_type value) { this->append(value); }
  void pop_back() { this->erase(cend() - 1); }
  void append(value_type value) { this->insert(cend(), value); }
  void append(const_iterator str, Index size) {
    this->insert(cend(), str, size);
  }
  inline void append(StringView str);

  String substr(size_type start, size_type size = -1) const {
    if (size == -1) {
      size = this->size_ - start;
    }
    return String(this->buffer_ + start, size);
  }

  String& operator+=(value_type value) {
    this->append(value);
    return *this;
  }
  String& operator+=(StringView other);

 public:
  operator std::u16string_view() const {
    return std::u16string_view(data(), size());
  }

  StringView View() const;

 public:
  Index Find(value_type value, Index start = 0) const;
  std::vector<String> Split(value_type separator,
                            bool remove_space_line = false) const;
  std::vector<String> SplitToLines(bool remove_space_line = false) const;

  bool StartWith(StringView str) const;
  bool EndWith(StringView str) const;

  String& TrimStart();
  String& TrimEnd();
  String& Trim();

 public:
  void AppendCodePoint(CodePoint code_point);

  Utf16CodePointIterator CodePointIterator() const {
    return Utf16CodePointIterator(buffer_, size_);
  }

  Index IndexFromCodeUnitToCodePoint(Index code_unit_index) const;
  Index IndexFromCodePointToCodeUnit(Index code_point_index) const;
  Range RangeFromCodeUnitToCodePoint(Range code_unit_range) const;
  Range RangeFromCodePointToCodeUnit(Range code_point_range) const;

#ifdef CRU_PLATFORM_WINDOWS
  const wchar_t* WinCStr() const {
    return reinterpret_cast<const wchar_t*>(c_str());
  }
#endif

  template <typename... T>
  String Format(T&&... args) const;

  std::string ToUtf8() const;
  Buffer ToUtf8Buffer(bool end_zero = true) const;

  int Compare(const String& other) const;
  int CaseInsensitiveCompare(const String& other) const;
  bool CaseInsensitiveEqual(const String& other) const {
    return CaseInsensitiveCompare(other) == 0;
  }

 private:
  char16_t* buffer_;
  Index size_;      // not including trailing '\0'
  Index capacity_;  // always 1 smaller than real buffer size
};
CRU_BASE_API 
std::ostream& operator<<(std::ostream& os, const String& value);

class CRU_BASE_API StringView {
 public:
  using value_type = char16_t;
  using size_type = Index;
  using difference_type = Index;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = const value_type*;
  using const_iterator = const value_type*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  StringView() = default;

  constexpr StringView(const_pointer ptr, Index size)
      : ptr_(ptr), size_(size) {}

  template <Index size>
  constexpr StringView(const value_type (&array)[size])
      : StringView(array, size - 1) {}

  StringView(const String& str) : StringView(str.data(), str.size()) {}

  CRU_DEFAULT_COPY(StringView)
  CRU_DEFAULT_MOVE(StringView)

  ~StringView() = default;

  bool empty() const { return size_ == 0; }
  Index size() const { return size_; }
  const value_type* data() const { return ptr_; }

 public:
  iterator begin() { return this->ptr_; }
  const_iterator begin() const { return this->ptr_; }
  const_iterator cbegin() const { return this->ptr_; }

  iterator end() { return this->ptr_ + this->size_; }
  const_iterator end() const { return this->ptr_ + this->size_; }
  const_iterator cend() const { return this->ptr_ + this->size_; }

  reverse_iterator rbegin() { return reverse_iterator{end()}; }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator{end()};
  }
  const_reverse_iterator crbegin() const {
    return const_reverse_iterator{cend()};
  }

  reverse_iterator rend() { return reverse_iterator{begin()}; }
  const_reverse_iterator rend() const {
    return const_reverse_iterator{begin()};
  }
  const_reverse_iterator crend() const {
    return const_reverse_iterator{cbegin()};
  }

  StringView substr(Index pos);
  StringView substr(Index pos, Index size);

  value_type operator[](Index index) const { return ptr_[index]; }

  operator std::u16string_view() const {
    return std::u16string_view(data(), size());
  }

 public:
  int Compare(const StringView& other) const;
  int CaseInsensitiveCompare(const StringView& other) const;
  bool CaseInsensitiveEqual(const StringView& other) const {
    return CaseInsensitiveCompare(other) == 0;
  }

  String ToString() const { return String(ptr_, size_); }

  Utf16CodePointIterator CodePointIterator() const {
    return Utf16CodePointIterator(ptr_, size_);
  }

  Index Find(value_type value, Index start = 0) const;
  std::vector<String> Split(value_type separator,
                            bool remove_space_line = false) const;
  std::vector<String> SplitToLines(bool remove_space_line = false) const;

  bool StartWith(StringView str) const;
  bool EndWith(StringView str) const;

  Index IndexFromCodeUnitToCodePoint(Index code_unit_index) const;
  Index IndexFromCodePointToCodeUnit(Index code_point_index) const;
  Range RangeFromCodeUnitToCodePoint(Range code_unit_range) const;
  Range RangeFromCodePointToCodeUnit(Range code_point_range) const;

  std::string ToUtf8() const;
  Buffer ToUtf8Buffer(bool end_zero = true) const;

 private:
  const char16_t* ptr_;
  Index size_;
};

CRU_DEFINE_COMPARE_OPERATORS(String)

inline String operator+(const String& left, const String& right) {
  String result;
  result += left;
  result += right;
  return result;
}

inline String operator+(String::value_type left, const String& right) {
  String result;
  result += left;
  result += right;
  return result;
}

inline String operator+(const String& left, String::value_type right) {
  String result;
  result += left;
  result += right;
  return result;
}

CRU_DEFINE_COMPARE_OPERATORS(StringView)

inline String::iterator String::insert(const_iterator pos, StringView str) {
  return insert(pos, str.data(), str.size());
}

inline void String::append(StringView str) {
  this->append(str.data(), str.size());
}

inline String String::From(StringView str) { return str.ToString(); }

inline String::String(StringView str) : String(str.data(), str.size()) {}

inline String ToString(StringView value) { return value.ToString(); }

inline CodePoint Utf16PreviousCodePoint(StringView str, Index current,
                                        Index* previous_position) {
  return Utf16PreviousCodePoint(str.data(), str.size(), current,
                                previous_position);
}

inline CodePoint Utf16NextCodePoint(StringView str, Index current,
                                    Index* next_position) {
  return Utf16NextCodePoint(str.data(), str.size(), current, next_position);
}

inline bool Utf16IsValidInsertPosition(StringView str, Index position) {
  return Utf16IsValidInsertPosition(str.data(), str.size(), position);
}

// Return position after the character making predicate returns true or 0 if no
// character doing so.
inline Index CRU_BASE_API
Utf16BackwardUntil(StringView str, Index position,
                   const std::function<bool(CodePoint)>& predicate) {
  return Utf16BackwardUntil(str.data(), str.size(), position, predicate);
}
// Return position before the character making predicate returns true or
// str.size() if no character doing so.
inline Index CRU_BASE_API
Utf16ForwardUntil(StringView str, Index position,
                  const std::function<bool(CodePoint)>& predicate) {
  return Utf16ForwardUntil(str.data(), str.size(), position, predicate);
}

inline Index Utf16PreviousWord(StringView str, Index position,
                               bool* is_space = nullptr) {
  return Utf16PreviousWord(str.data(), str.size(), position, is_space);
}

inline Index Utf16NextWord(StringView str, Index position,
                           bool* is_space = nullptr) {
  return Utf16NextWord(str.data(), str.size(), position, is_space);
}

String CRU_BASE_API ToLower(StringView s);
String CRU_BASE_API ToUpper(StringView s);

}  // namespace cru

template <>
struct std::hash<cru::String> {
  std::size_t operator()(const cru::String& value) const {
    return std::hash<std::u16string_view>{}(std::u16string_view(
        reinterpret_cast<const char16_t*>(value.data()), value.size()));
  }
};

template <>
struct std::hash<cru::StringView> {
  std::size_t operator()(const cru::StringView& value) const {
    return std::hash<std::u16string_view>{}(std::u16string_view(
        reinterpret_cast<const char16_t*>(value.data()), value.size()));
  }
};
