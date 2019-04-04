#pragma once
#include "pre_config.hpp"

#include <string>
#include <string_view>

namespace cru::util {
namespace details {
template <typename T>
struct TypeTag {};

constexpr std::wstring_view PlaceHolder(TypeTag<std::wstring>) {
  return std::wstring_view(L"{}");
}

constexpr std::string_view PlaceHolder(TypeTag<std::string>) {
  return std::string_view("{}");
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
std::wstring Format(const std::wstring_view& format, const T&... args) {
  std::wstring result(format);
  details::FormatInternal<std::wstring>(result, args...);
  return result;
}

template <typename... T>
std::string Format(const std::string_view& format, const T&... args) {
  std::string result(format);
  details::FormatInternal<std::string>(result, args...);
  return result;
}

#define CRU_FORMAT_NUMBER(type)                                        \
  inline std::string FormatToString(const type number,                 \
                                    details::TypeTag<std::string>) {   \
    return std::to_string(number);                                     \
  }                                                                    \
  inline std::wstring FormatToString(const type number,                \
                                     details::TypeTag<std::wstring>) { \
    return std::to_wstring(number);                                    \
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

inline std::wstring_view FormatToString(const std::wstring& string,
                                        details::TypeTag<std::wstring>) {
  return string;
}

inline std::string_view FormatToString(const std::string& string,
                                       details::TypeTag<std::string>) {
  return string;
}

inline std::wstring_view FormatToString(const std::wstring_view& string,
                                        details::TypeTag<std::wstring>) {
  return string;
}

inline std::string_view FormatToString(const std::string_view& string,
                                       details::TypeTag<std::string>) {
  return string;
}

inline std::wstring_view FormatToString(const wchar_t* string,
                                        details::TypeTag<std::wstring>) {
  return std::wstring_view(string);
}

inline std::string_view FormatToString(const char* string,
                                       details::TypeTag<std::string>) {
  return std::string(string);
}
}  // namespace cru::util
