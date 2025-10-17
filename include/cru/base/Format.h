#pragma once

#include "Exception.h"
#include "String.h"

#include <cassert>
#include <cstdio>
#include <format>
#include <type_traits>
#include <vector>

namespace cru {
inline String ToString(bool value) {
  return value ? String(u"true") : String(u"false");
}

template <typename T>
inline constexpr std::nullptr_t kPrintfFormatSpecifierOfType = nullptr;

#define CRU_DEFINE_PRINTF_FORMAT_SPECIFIER_OF_TYPE(type, specifier) \
  template <>                                                       \
  inline constexpr const char* kPrintfFormatSpecifierOfType<type> = specifier;

CRU_DEFINE_PRINTF_FORMAT_SPECIFIER_OF_TYPE(signed char, "%c")
CRU_DEFINE_PRINTF_FORMAT_SPECIFIER_OF_TYPE(unsigned char, "%c")
CRU_DEFINE_PRINTF_FORMAT_SPECIFIER_OF_TYPE(signed short, "%hd")
CRU_DEFINE_PRINTF_FORMAT_SPECIFIER_OF_TYPE(unsigned short, "%hu")
CRU_DEFINE_PRINTF_FORMAT_SPECIFIER_OF_TYPE(signed int, "%d")
CRU_DEFINE_PRINTF_FORMAT_SPECIFIER_OF_TYPE(unsigned int, "%u")
CRU_DEFINE_PRINTF_FORMAT_SPECIFIER_OF_TYPE(signed long, "%ld")
CRU_DEFINE_PRINTF_FORMAT_SPECIFIER_OF_TYPE(unsigned long, "%lu")
CRU_DEFINE_PRINTF_FORMAT_SPECIFIER_OF_TYPE(signed long long, "%lld")
CRU_DEFINE_PRINTF_FORMAT_SPECIFIER_OF_TYPE(unsigned long long, "%llu")
CRU_DEFINE_PRINTF_FORMAT_SPECIFIER_OF_TYPE(float, "%f")
CRU_DEFINE_PRINTF_FORMAT_SPECIFIER_OF_TYPE(double, "%f")

#undef CRU_DEFINE_PRINTF_FORMAT_SPECIFIER_OF_TYPE

template <typename T>
std::enable_if_t<
    !std::is_null_pointer_v<decltype(kPrintfFormatSpecifierOfType<T>)>, String>
ToString(T value) {
  auto size = std::snprintf(nullptr, 0, kPrintfFormatSpecifierOfType<T>, value);
  assert(size > 0);
  std::vector<char> buffer(size + 1);
  size = std::snprintf(buffer.data(), size + 1, kPrintfFormatSpecifierOfType<T>,
                       value);
  assert(size > 0);
  return String::FromUtf8(buffer.data(), size);
}

template <typename T>
String ToString(const T& value, StringView option) {
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

std::vector<FormatToken> CRU_BASE_API ParseToFormatTokenList(StringView str);

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
            "Currently do not support positional or named place holder.");
      }
    } else {
      current += token.data;
    }
  }
}
}  // namespace details

template <typename... T>
String Format(StringView format, T&&... args) {
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

template <typename T>
struct ImplementFormatterByToUtf8String {
  template <class ParseContext>
  constexpr ParseContext::iterator parse(ParseContext& ctx) const {
    auto iter = ctx.begin();
    if (*iter != '}') {
      throw std::format_error(
          "ImplementFormatterByToUtf8String does not accept format args.");
    }
    return iter;
  }

  template <class FmtContext>
  FmtContext::iterator format(const T& object, FmtContext& ctx) const {
    return std::ranges::copy(ToUtf8String(object), ctx.out()).out;
  }
};
}  // namespace cru
