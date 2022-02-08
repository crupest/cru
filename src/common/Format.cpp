#include "cru/common/Format.h"

namespace cru {
namespace details {
FormatToken ParsePlaceHolder(String place_holder_string) {
  if (place_holder_string.empty()) {
    return FormatToken::NonePlaceHolder({});
  }

  if (place_holder_string.StartWith(u":")) {
    if (place_holder_string.Find(u':', 1) != -1) {
      throw Exception(u"Two ':' inside placeholder.");
    }

    return FormatToken::NonePlaceHolder(place_holder_string.substr(1));
  }
  if (IsDigit(place_holder_string[0])) {
    int position = 0;
    int index = 0;
    while (index < place_holder_string.size() &&
           IsDigit(place_holder_string[index])) {
      position = position * 10 + place_holder_string[index] - '0';
      index++;
    }

    String option;

    if (index != place_holder_string.size()) {
      if (place_holder_string[index] != ':') {
        throw Exception(u"Invalid placeholder in format.");
      }

      option = place_holder_string.substr(index + 1);
    }

    return FormatToken::PositionedPlaceHolder(position, std::move(option));
  }

  auto separator_index = place_holder_string.Find(':');
  if (separator_index == -1) {
    return FormatToken::NamedPlaceHolder(place_holder_string, {});
  } else {
    return FormatToken::NamedPlaceHolder(
        place_holder_string.substr(0, separator_index),
        place_holder_string.substr(separator_index + 1));
  }
}

std::vector<FormatToken> ParseToFormatTokenList(const String& str) {
  std::vector<FormatToken> result;

  auto push_char = [&result](char16_t c) {
    if (result.empty() || result.back().type == FormatTokenType::PlaceHolder) {
      result.push_back(FormatToken::Text());
    }
    result.back().data.append(c);
  };

  bool try_to_escape = false;
  bool is_in_place_holder = false;
  String place_holder_string;

  for (auto c : str) {
    if (c == u'{') {
      if (try_to_escape) {
        push_char(u'{');
        try_to_escape = false;
        is_in_place_holder = false;
      } else {
        if (is_in_place_holder) {
          throw Exception(u"Invalid format string: '{' inside placeholder.");
        }

        try_to_escape = true;
        is_in_place_holder = true;
      }
    } else if (c == u'}') {
      if (is_in_place_holder) {
        is_in_place_holder = false;
        result.push_back(ParsePlaceHolder(std::move(place_holder_string)));
        place_holder_string.clear();
      } else {
        push_char(u'}');
      }
      try_to_escape = false;
    } else {
      if (is_in_place_holder) {
        place_holder_string.push_back(c);
      } else {
        push_char(c);
      }
      try_to_escape = false;
    }
  }
  return result;
}

void FormatAppendFromFormatTokenList(
    String& current, const std::vector<FormatToken>& format_token_list,
    Index index) {
  for (Index i = index; i < static_cast<Index>(format_token_list.size()); i++) {
    const auto& token = format_token_list[i];
    if (token.type == FormatTokenType::PlaceHolder) {
      throw Exception(u"More placeholder than args.");
    } else {
      current += token.data;
    }
  }
}
}  // namespace details
}  // namespace cru
