#pragma once
#include "pre_config.hpp"

#include <sstream>
#include <string>
#include <string_view>

namespace cru::util {
namespace details {
template <typename TChar>
struct FormatTrait {};

template <>
struct FormatTrait<char> {
  using StringType = std::string;
  using ViewType = std::string_view;
  using StreamType = std::stringstream;
  static constexpr ViewType placeholder = "{}";
};

template <>
struct FormatTrait<wchar_t> {
  using StringType = std::wstring;
  using ViewType = std::wstring_view;
  using StreamType = std::wstringstream;
  static constexpr ViewType placeholder = L"{}";
};
}  // namespace details

template <typename TFormatTrait, typename T>
struct Formatter {
  static typename TFormatTrait::StringType Format(const T& value) {
    typename TFormatTrait::StreamType stream;
    stream << value;
    return stream.str();
  }
};

namespace details {
template <typename TString>
void FormatInternal(TString& string) {
  using Trait = FormatTrait<TString::value_type>;
  constexpr const auto& placeholder = Trait::placeholder;

  const auto find_result = string.find(placeholder);
  if (find_result != TString::npos)
    throw std::invalid_argument("There is more placeholders than args.");
}

template <typename TString, typename T, typename... TRest>
void FormatInternal(TString& string, const T& arg, const TRest&... args) {
  using Trait = FormatTrait<TString::value_type>;
  constexpr const auto& placeholder = Trait::placeholder;

  const auto find_result = string.find(placeholder);
  if (find_result == TString::npos)
    throw std::invalid_argument("There is less placeholders than args.");

  string.replace(find_result, 2, Formatter<Trait, T>::Format(arg));
  FormatInternal<TString>(string, args...);
}
}  // namespace details

template <typename... T>
std::wstring Format(std::wstring format, const T&... args) {
  details::FormatInternal(format, args...);
  return format;
}

template <typename... T>
std::string Format(std::string format, const T&... args) {
  details::FormatInternal(format, args...);
  return format;
}

// Why is two overloads below exist?
// Because people should be able to pass string_view instance as format.
// However, the two overloads above do not accept string_view as format due to
// conversion from string_view to string is explicit.
//
// Why not just overload but SFINAE?
// Because I want to make two overloads below worse than the two ones above.
// Otherwise it will be ambiguous when pass const char* as format.
//

template <typename T, typename... TArgs>
auto Format(T format, const TArgs&... args)
    -> std::enable_if_t<std::is_same_v<T, std::wstring_view>, std::wstring> {
  return Format(std::wstring{format}, args...);
}

template <typename T, typename... TArgs>
auto Format(T format, const TArgs&... args)
    -> std::enable_if_t<std::is_same_v<T, std::string_view>, std::string> {
  return Format(std::string{format}, args...);
}
}  // namespace cru::util
