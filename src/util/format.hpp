#pragma once
#include "pre.hpp"

#include "base.hpp"

namespace cru::util {
namespace details {
template <typename T>
struct TypeTag {};

constexpr StringView PlaceHolder(TypeTag<String>) { return StringView(L"{}"); }

constexpr MultiByteStringView PlaceHolder(TypeTag<MultiByteString>) {
  return MultiByteStringView("{}");
}

template <typename TString>
void FormatInternal(TString& string) {
  const auto find_result = string.find(PlaceHolder(TypeTag<TString>{}));
  if (find_result != TString::npos)
    throw std::invalid_argument("There is more placeholders than args.");
}

template <typename TString, typename T, typename... TRest>
void FormatInternal(TString& string, const T& arg, const TRest&... args) {
  const auto find_result = string.find(PlaceHolder(TypeTag<TString>{}));
  if (find_result == TString::npos)
    throw std::invalid_argument("There is less placeholders than args.");

  string.replace(find_result, 2, FormatToString(arg, TypeTag<TString>{}));
  FormatInternal<TString>(string, args...);
}
}  // namespace details

template <typename... T>
String Format(const StringView& format, const T&... args) {
  String result(format);
  details::FormatInternal<String>(result, args...);
  return result;
}

template <typename... T>
MultiByteString Format(const MultiByteStringView& format, const T&... args) {
  MultiByteString result(format);
  details::FormatInternal<MultiByteString>(result, args...);
  return result;
}

#define CRU_FORMAT_NUMBER(type)                                               \
  inline String FormatToString(const type number, details::TypeTag<String>) { \
    return std::to_wstring(number);                                           \
  }                                                                           \
  inline MultiByteString FormatToString(const type number,                    \
                                        details::TypeTag<MultiByteString>) {  \
    return std::to_string(number);                                            \
  }

CRU_FORMAT_NUMBER(int)
CRU_FORMAT_NUMBER(short)
CRU_FORMAT_NUMBER(long)
CRU_FORMAT_NUMBER(long long)
CRU_FORMAT_NUMBER(unsigned int)
CRU_FORMAT_NUMBER(unsigned short)
CRU_FORMAT_NUMBER(unsigned long)
CRU_FORMAT_NUMBER(unsigned long long)
CRU_FORMAT_NUMBER(float)
CRU_FORMAT_NUMBER(double)

#undef CRU_FORMAT_NUMBER

inline StringView FormatToString(const String& string,
                                 details::TypeTag<String>) {
  return string;
}

inline MultiByteString FormatToString(const MultiByteString& string,
                                      details::TypeTag<MultiByteString>) {
  return string;
}

inline StringView FormatToString(const StringView& string,
                                 details::TypeTag<String>) {
  return string;
}

inline MultiByteStringView FormatToString(const MultiByteStringView& string,
                                          details::TypeTag<MultiByteString>) {
  return string;
}

inline StringView FormatToString(const wchar_t* string,
                                 details::TypeTag<String>) {
  return StringView(string);
}

inline MultiByteStringView FormatToString(const char* string,
                                          details::TypeTag<MultiByteString>) {
  return MultiByteString(string);
}
}  // namespace cru::util
