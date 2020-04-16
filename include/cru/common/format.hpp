#pragma once
#include "pre_config.hpp"

#include <sstream>
#include <string>
#include <string_view>

namespace cru::util {
namespace details {
template <typename T>
struct FormatTrait {};

template <>
struct FormatTrait<std::string_view> {
  static constexpr std::string_view placeholder = "{}";
  using ResultType = std::string;
  using StreamType = std::stringstream;
};

template <>
struct FormatTrait<std::wstring_view> {
  static constexpr std::wstring_view placeholder = L"{}";
  using ResultType = std::wstring;
  using StreamType = std::wstringstream;
};

template <typename TStringView>
void FormatInternal(typename FormatTrait<TStringView>::StreamType& stream,
                    const TStringView& string) {
  const auto find_result = string.find(FormatTrait<TStringView>::placeholder);
  if (find_result != TStringView::npos)
    throw std::invalid_argument("There is more placeholders than args.");
  stream << string;
}

template <typename TStringView, typename T, typename... TRest>
void FormatInternal(typename FormatTrait<TStringView>::StreamType& stream,
                    const TStringView& string, const T& arg,
                    const TRest&... args) {
  const auto find_result = string.find(FormatTrait<TStringView>::placeholder);
  if (find_result == TStringView::npos)
    throw std::invalid_argument("There is less placeholders than args.");

  stream << string.substr(0, find_result);
  stream << arg;

  FormatInternal(stream, string.substr(find_result + 2), args...);
}

template <typename TStringView, typename... T>
auto FormatTemplate(const TStringView& format, const T&... args) ->
    typename FormatTrait<TStringView>::ResultType {
  typename FormatTrait<TStringView>::StreamType stream;
  FormatInternal(stream, format, args...);
  return stream.str();
}
}  // namespace details

template <typename... T>
std::wstring Format(const std::wstring_view& format, const T&... args) {
  return details::FormatTemplate(format, args...);
}

template <typename... T>
std::string Format(const std::string_view& format, const T&... args) {
  return details::FormatTemplate(format, args...);
}
}  // namespace cru::util
