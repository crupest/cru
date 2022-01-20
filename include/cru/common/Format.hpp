#pragma once

#include "Exception.hpp"
#include "String.hpp"

#include <charconv>

namespace cru {
inline String ToString(bool value) {
  return value ? String(u"true") : String(u"false");
}

template <typename T>
std::enable_if_t<std::is_integral_v<T>, String> ToString(T value) {
  std::array<char, 50> buffer;
  auto result =
      std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);

  if (result.ec == std::errc{}) {
  } else {
    throw std::invalid_argument("Failed to convert value to chars.");
  }

  auto size = result.ptr - buffer.data();
  auto b = new char16_t[size + 1];
  b[size] = 0;
  std::copy(buffer.data(), result.ptr, b);
  return String::FromBuffer(b, size, size);
}

template <typename T>
std::enable_if_t<std::is_floating_point_v<T>, String> ToString(T value) {
  auto str = std::to_string(value);
  return String(str.cbegin(), str.cend());
}

template <typename T>
std::enable_if_t<
    std::is_convertible_v<decltype(ToString(std::declval<const T&>)), String>,
    String>
ToString(const T& value, StringView option) {
  CRU_UNUSED(option)
  return ToString(value);
}

inline String ToString(String value) { return value; }

namespace details {
enum class FormatTokenType { PlaceHolder, Text };
enum class FormatPlaceHolderType { None, Positioned, Named };

struct FormatToken {
  static FormatToken Text() {
    return FormatToken{FormatTokenType::Text, {}, {}, 0, {}, {}};
  }

  static FormatToken NonePlaceHolder(String option) {
    return FormatToken(FormatTokenType::PlaceHolder, {},
                       FormatPlaceHolderType::None, 0, {}, std::move(option));
  }

  static FormatToken PositionedPlaceHolder(int position, String option) {
    return FormatToken(FormatTokenType::PlaceHolder, {},
                       FormatPlaceHolderType::Positioned, position, {},
                       std::move(option));
  }

  static FormatToken NamedPlaceHolder(String name, String option) {
    return FormatToken(FormatTokenType::PlaceHolder, {},
                       FormatPlaceHolderType::Named, 0, std::move(name),
                       std::move(option));
  }

  FormatToken(FormatTokenType type, String data,
              FormatPlaceHolderType place_holder_type,
              int place_holder_position, String place_holder_name,
              String place_holder_option)
      : type(type),
        data(std::move(data)),
        place_holder_type(place_holder_type),
        place_holder_position(place_holder_position),
        place_holder_name(std::move(place_holder_name)),
        place_holder_option(std::move(place_holder_option)) {}

  CRU_DEFAULT_COPY(FormatToken)
  CRU_DEFAULT_MOVE(FormatToken)

  CRU_DEFAULT_DESTRUCTOR(FormatToken)

  FormatTokenType type;
  String data;
  FormatPlaceHolderType place_holder_type;
  int place_holder_position;
  String place_holder_name;
  String place_holder_option;
};

std::vector<FormatToken> CRU_BASE_API ParseToFormatTokenList(const String& str);

void CRU_BASE_API FormatAppendFromFormatTokenList(
    String& current, const std::vector<FormatToken>& format_token_list,
    Index index);

template <typename TA, typename... T>
void FormatAppendFromFormatTokenList(
    String& current, const std::vector<FormatToken>& format_token_list,
    Index index, TA&& args0, T&&... args) {
  for (Index i = index; i < static_cast<Index>(format_token_list.size()); i++) {
    const auto& token = format_token_list[i];
    if (token.type == FormatTokenType::PlaceHolder) {
      if (token.place_holder_type == FormatPlaceHolderType::None) {
        current += ToString(std::forward<TA>(args0), token.place_holder_option);
        FormatAppendFromFormatTokenList(current, format_token_list, i + 1,
                                        std::forward<T>(args)...);

        return;
      } else {
        throw Exception(
            u"Currently do not support positional or named place holder.");
      }
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
}  // namespace cru
