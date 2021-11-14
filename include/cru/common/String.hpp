#pragma once
#include "Base.hpp"

#include "Range.hpp"
#include "StringUtil.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
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
  static String FromUtf8(std::string_view str) {
    return FromUtf8(str.data(), str.size());
  }

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

#ifdef CRU_PLATFORM_WINDOWS
  static String FromUtf16(wchar_t* str) { return String(str); }
  static String FromUtf16(wchar_t* str, Index size) {
    return String(str, size);
  }
#endif

 public:
  String() = default;

  explicit String(const_pointer str);
  String(const_pointer str, size_type size);

  template <Index size>
  String(const char16_t (&str)[size])
      : String(reinterpret_cast<const_pointer>(str), size - 1) {}

  template <typename Iter>
  String(Iter start, Iter end) {
    for (; start != end; start++) {
      append(*start);
    }
  }

  String(std::initializer_list<value_type> l);

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

 public:
  String& operator+=(StringView other);

  operator std::u16string_view() const {
    return std::u16string_view(data(), size());
  }

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

  int Compare(const String& other) const;

 private:
  static char16_t kEmptyBuffer[1];

 private:
  char16_t* buffer_ = kEmptyBuffer;
  Index size_ = 0;      // not including trailing '\0'
  Index capacity_ = 0;  // always 1 smaller than real buffer size
};

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

  Index size() const { return size_; }
  const value_type* data() const { return ptr_; }

 public:
  iterator begin() { return this->ptr_; }
  const_iterator begin() const { return this->ptr_; }
  const_iterator cbegin() const { return this->ptr_; }

  iterator end() { return this->ptr_ + this->size_; }
  const_iterator end() const { return this->ptr_ + this->size_; }
  const_iterator cend() const { return this->ptr_ + this->size_; }

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

  StringView substr(Index pos);
  StringView substr(Index pos, Index size);

  int Compare(const StringView& other) const;

  String ToString() const { return String(ptr_, size_); }

  value_type operator[](Index index) const { return ptr_[index]; }

  operator std::u16string_view() const {
    return std::u16string_view(data(), size());
  }

  std::string ToUtf8() const;

 private:
  const char16_t* ptr_;
  Index size_;
};

CRU_DEFINE_COMPARE_OPERATORS(String)

inline String operator+(const String& left, const String& right) {
  String result(left);
  result += right;
  return result;
}

inline String ToString(bool value) {
  return value ? String(u"true") : String(u"false");
}

template <typename T>
std::enable_if_t<std::is_integral_v<T>, String> ToString(T value) {
  std::array<char, 50> buffer;
  auto result =
      std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);

  auto size = result.ptr - buffer.data();
  auto b = new char16_t[size + 1];
  b[size] = 0;
  std::copy(buffer.data(), result.ptr, b);
  return String::FromBuffer(b, size, size);

  if (result.ec == std::errc{}) {
  } else {
    throw std::invalid_argument("Failed to convert value to chars.");
  }
}

template <typename T>
std::enable_if_t<std::is_floating_point_v<T>, String> ToString(T value) {
  auto str = std::to_string(value);
  return String(str.cbegin(), str.cend());
}

inline String ToString(String value) { return std::move(value); }

namespace details {
enum class FormatTokenType { PlaceHolder, Text };

struct FormatToken {
  FormatTokenType type;
  String data;
};

std::vector<FormatToken> CRU_BASE_API ParseToFormatTokenList(const String& str);

void CRU_BASE_API FormatAppendFromFormatTokenList(
    String& current, const std::vector<FormatToken>& format_token_list,
    Index index);

template <typename TA, typename... T>
void FormatAppendFromFormatTokenList(
    String& current, const std::vector<FormatToken>& format_token_list,
    Index index, TA&& args0, T&&... args) {
  for (Index i = index; i < format_token_list.size(); i++) {
    const auto& token = format_token_list[i];
    if (token.type == FormatTokenType::PlaceHolder) {
      current += ToString(std::forward<TA>(args0));
      FormatAppendFromFormatTokenList(current, format_token_list, i + 1,
                                      std::forward<T>(args)...);
      return;
    } else {
      current += token.data;
    }
  }
}
}  // namespace details

template <typename... T>
String Format(const String& format, T&&... args) {
  String result;

  details::FormatAppendFromFormatTokenList(
      result, details::ParseToFormatTokenList(format), 0,
      std::forward<T>(args)...);

  return result;
}

template <typename... T>
String String::Format(T&&... args) const {
  return cru::Format(*this, std::forward<T>(args)...);
}

CRU_DEFINE_COMPARE_OPERATORS(StringView)

inline String::iterator String::insert(const_iterator pos, StringView str) {
  return insert(pos, str.data(), str.size());
}

inline void String::append(StringView str) {
  this->append(str.data(), str.size());
}

inline String String::From(StringView str) { return str.ToString(); }

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
