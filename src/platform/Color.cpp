#include "cru/platform/Color.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <gsl/gsl>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace cru::platform {
String Color::ToString() const {
  auto to_hex = [](std::uint8_t v) -> char16_t {
    return v >= 10 ? v - 10 + u'a' : v + u'0';
  };

  auto to_two_hex_digit = [to_hex](std::uint8_t v) -> String {
    return {to_hex(v /= 16), to_hex(v %= 16)};
  };

  String result = u"#";
  result.append(to_two_hex_digit(red));
  result.append(to_two_hex_digit(green));
  result.append(to_two_hex_digit(blue));
  result.append(to_two_hex_digit(alpha));
  return result;
}

std::optional<Color> Color::Parse(StringView string,
                                  bool parse_predefined_color) {
  if (parse_predefined_color) {
    auto optional_predefined_color = GetPredefinedColorByName(string);
    if (optional_predefined_color) {
      return *optional_predefined_color;
    }
  }

  auto get_num = [](char16_t c) -> std::optional<int> {
    if (c >= u'0' && c <= u'9') return c - u'0';
    if (c >= u'A' && c <= u'F') return c - u'A' + 10;
    if (c >= u'a' && c <= u'f') return c - u'a' + 10;
    return std::nullopt;
  };

  auto get_num_for_two_digit = [get_num](StringView str) -> std::optional<int> {
    int num = 0;
    auto d1 = get_num(str[0]);
    if (!d1) return std::nullopt;
    num += *d1 * 16;
    auto d2 = get_num(str[1]);
    if (!d2) return std::nullopt;
    num += *d2;
    return num;
  };

  const auto string_size = string.size();

  if (string_size == 7) {
    if (string[0] != u'#') return std::nullopt;
    auto r = get_num_for_two_digit(string.substr(1, 2));
    if (!r) return std::nullopt;
    auto g = get_num_for_two_digit(string.substr(3, 2));
    if (!g) return std::nullopt;
    auto b = get_num_for_two_digit(string.substr(5, 2));
    if (!b) return std::nullopt;
    return Color(gsl::narrow_cast<std::uint8_t>(*r),
                 gsl::narrow_cast<std::uint8_t>(*g),
                 gsl::narrow_cast<std::uint8_t>(*b));
  } else if (string_size == 9) {
    if (string[0] != u'#') return std::nullopt;
    auto r = get_num_for_two_digit(string.substr(1, 2));
    if (!r) return std::nullopt;
    auto g = get_num_for_two_digit(string.substr(3, 2));
    if (!g) return std::nullopt;
    auto b = get_num_for_two_digit(string.substr(5, 2));
    if (!b) return std::nullopt;
    auto a = get_num_for_two_digit(string.substr(7, 2));
    if (!a) return std::nullopt;
    return Color(
        gsl::narrow_cast<std::uint8_t>(*r), gsl::narrow_cast<std::uint8_t>(*g),
        gsl::narrow_cast<std::uint8_t>(*b), gsl::narrow_cast<std::uint8_t>(*a));
  } else {
    return std::nullopt;
  }
}

namespace details {
const std::unordered_map<StringView, Color> predefined_name_color_map{
    {u"transparent", colors::transparent},
    {u"black", colors::black},
    {u"silver", colors::silver},
    {u"gray", colors::gray},
    {u"white", colors::white},
    {u"maroon", colors::maroon},
    {u"red", colors::red},
    {u"purple", colors::purple},
    {u"fuchsia", colors::fuchsia},
    {u"green", colors::green},
    {u"lime", colors::lime},
    {u"olive", colors::olive},
    {u"yellow", colors::yellow},
    {u"navy", colors::navy},
    {u"blue", colors::blue},
    {u"teal", colors::teal},
    {u"aqua", colors::aqua},
    {u"orange", colors::orange},
    {u"aliceblue", colors::aliceblue},
    {u"antiquewhite", colors::antiquewhite},
    {u"aquamarine", colors::aquamarine},
    {u"azure", colors::azure},
    {u"beige", colors::beige},
    {u"bisque", colors::bisque},
    {u"blanchedalmond", colors::blanchedalmond},
    {u"blueviolet", colors::blueviolet},
    {u"brown", colors::brown},
    {u"burlywood", colors::burlywood},
    {u"cadetblue", colors::cadetblue},
    {u"chartreuse", colors::chartreuse},
    {u"chocolate", colors::chocolate},
    {u"coral", colors::coral},
    {u"cornflowerblue", colors::cornflowerblue},
    {u"cornsilk", colors::cornsilk},
    {u"crimson", colors::crimson},
    {u"cyan", colors::cyan},
    {u"darkblue", colors::darkblue},
    {u"darkcyan", colors::darkcyan},
    {u"darkgoldenrod", colors::darkgoldenrod},
    {u"darkgray", colors::darkgray},
    {u"darkgreen", colors::darkgreen},
    {u"darkgrey", colors::darkgrey},
    {u"darkkhaki", colors::darkkhaki},
    {u"darkmagenta", colors::darkmagenta},
    {u"darkolivegreen", colors::darkolivegreen},
    {u"darkorange", colors::darkorange},
    {u"darkorchid", colors::darkorchid},
    {u"darkred", colors::darkred},
    {u"darksalmon", colors::darksalmon},
    {u"darkseagreen", colors::darkseagreen},
    {u"darkslateblue", colors::darkslateblue},
    {u"darkslategray", colors::darkslategray},
    {u"darkslategrey", colors::darkslategrey},
    {u"darkturquoise", colors::darkturquoise},
    {u"darkviolet", colors::darkviolet},
    {u"deeppink", colors::deeppink},
    {u"deepskyblue", colors::deepskyblue},
    {u"dimgray", colors::dimgray},
    {u"dimgrey", colors::dimgrey},
    {u"dodgerblue", colors::dodgerblue},
    {u"firebrick", colors::firebrick},
    {u"floralwhite", colors::floralwhite},
    {u"forestgreen", colors::forestgreen},
    {u"gainsboro", colors::gainsboro},
    {u"ghostwhite", colors::ghostwhite},
    {u"gold", colors::gold},
    {u"goldenrod", colors::goldenrod},
    {u"greenyellow", colors::greenyellow},
    {u"grey", colors::grey},
    {u"honeydew", colors::honeydew},
    {u"hotpink", colors::hotpink},
    {u"indianred", colors::indianred},
    {u"indigo", colors::indigo},
    {u"ivory", colors::ivory},
    {u"khaki", colors::khaki},
    {u"lavender", colors::lavender},
    {u"lavenderblush", colors::lavenderblush},
    {u"lawngreen", colors::lawngreen},
    {u"lemonchiffon", colors::lemonchiffon},
    {u"lightblue", colors::lightblue},
    {u"lightcoral", colors::lightcoral},
    {u"lightcyan", colors::lightcyan},
    {u"lightgoldenrodyellow", colors::lightgoldenrodyellow},
    {u"lightgray", colors::lightgray},
    {u"lightgreen", colors::lightgreen},
    {u"lightgrey", colors::lightgrey},
    {u"lightpink", colors::lightpink},
    {u"lightsalmon", colors::lightsalmon},
    {u"lightseagreen", colors::lightseagreen},
    {u"lightskyblue", colors::lightskyblue},
    {u"lightslategray", colors::lightslategray},
    {u"lightslategrey", colors::lightslategrey},
    {u"lightsteelblue", colors::lightsteelblue},
    {u"lightyellow", colors::lightyellow},
    {u"limegreen", colors::limegreen},
    {u"linen", colors::linen},
    {u"magenta", colors::magenta},
    {u"mediumaquamarine", colors::mediumaquamarine},
    {u"mediumblue", colors::mediumblue},
    {u"mediumorchid", colors::mediumorchid},
    {u"mediumpurple", colors::mediumpurple},
    {u"mediumseagreen", colors::mediumseagreen},
    {u"mediumslateblue", colors::mediumslateblue},
    {u"mediumspringgreen", colors::mediumspringgreen},
    {u"mediumturquoise", colors::mediumturquoise},
    {u"mediumvioletred", colors::mediumvioletred},
    {u"midnightblue", colors::midnightblue},
    {u"mintcream", colors::mintcream},
    {u"mistyrose", colors::mistyrose},
    {u"moccasin", colors::moccasin},
    {u"navajowhite", colors::navajowhite},
    {u"oldlace", colors::oldlace},
    {u"olivedrab", colors::olivedrab},
    {u"orangered", colors::orangered},
    {u"orchid", colors::orchid},
    {u"palegoldenrod", colors::palegoldenrod},
    {u"palegreen", colors::palegreen},
    {u"paleturquoise", colors::paleturquoise},
    {u"palevioletred", colors::palevioletred},
    {u"papayawhip", colors::papayawhip},
    {u"peachpuff", colors::peachpuff},
    {u"peru", colors::peru},
    {u"pink", colors::pink},
    {u"plum", colors::plum},
    {u"powderblue", colors::powderblue},
    {u"rosybrown", colors::rosybrown},
    {u"royalblue", colors::royalblue},
    {u"saddlebrown", colors::saddlebrown},
    {u"salmon", colors::salmon},
    {u"sandybrown", colors::sandybrown},
    {u"seagreen", colors::seagreen},
    {u"seashell", colors::seashell},
    {u"sienna", colors::sienna},
    {u"skyblue", colors::skyblue},
    {u"slateblue", colors::slateblue},
    {u"slategray", colors::slategray},
    {u"slategrey", colors::slategrey},
    {u"snow", colors::snow},
    {u"springgreen", colors::springgreen},
    {u"steelblue", colors::steelblue},
    {u"tan", colors::tan},
    {u"thistle", colors::thistle},
    {u"tomato", colors::tomato},
    {u"turquoise", colors::turquoise},
    {u"violet", colors::violet},
    {u"wheat", colors::wheat},
    {u"whitesmoke", colors::whitesmoke},
    {u"yellowgreen", colors::yellowgreen},
    {u"rebeccapurple", colors::rebeccapurple},
};
}  // namespace details

std::optional<Color> GetPredefinedColorByName(StringView name) {
  auto result = details::predefined_name_color_map.find(name);
  if (result != details::predefined_name_color_map.cend()) {
    return result->second;
  } else {
    return std::nullopt;
  }
}

HslColor::HslColor(const Color& color) {
  float rgb[3] = {color.red / 255.f, color.green / 255.f, color.blue / 255.f};
  float& r = rgb[0];
  float& g = rgb[1];
  float& b = rgb[2];

  int max_index = 0;
  int min_index = 0;

  if (r > g) {
    max_index = r > b ? 0 : 2;
    min_index = g < b ? 1 : 2;
  } else {
    max_index = g > b ? 1 : 2;
    min_index = r < b ? 0 : 2;
  }

  float max = rgb[max_index];
  float min = rgb[min_index];

  float delta = max - min;

  if (delta == 0) {
    h = 0.f;
  } else if (max_index == 0) {
    h = 60.f * std::fmod((g - b) / delta, 6.f);
  } else if (max_index == 1) {
    h = 60.f * ((b - r) / delta + 2.f);
  } else {
    h = 60.f * ((r - g) / delta + 4.f);
  }

  l = (max + min) / 2.f;

  s = delta == 0 ? 0 : delta / (1 - std::abs(2 * l - 1));

  a = color.alpha / 255.f;
}

HslColor::operator Color() const {
  Expects(h >= 0.f && h <= 360.f);
  Expects(s >= 0.f && s <= 1.f);
  Expects(l >= 0.f && l <= 1.f);

  float c = (1 - std::abs(2 * l - 1)) * s;
  float x = c * (1 - std::abs(std::fmod(h / 60.f, 2.f) - 1));
  float m = l - c / 2;

  float rgb[3] = {0.f, 0.f, 0.f};

  if (h >= 0.f && h < 60.f) {
    rgb[0] = c;
    rgb[1] = x;
  } else if (h >= 60.f && h < 120.f) {
    rgb[0] = x;
    rgb[1] = c;
  } else if (h >= 120.f && h < 180.f) {
    rgb[1] = c;
    rgb[2] = x;
  } else if (h >= 180.f && h < 240.f) {
    rgb[1] = x;
    rgb[2] = c;
  } else if (h >= 240.f && h < 300.f) {
    rgb[0] = x;
    rgb[2] = c;
  } else if (h >= 300.f && h < 360.f) {
    rgb[0] = c;
    rgb[2] = x;
  }

  for (int i = 0; i < 3; ++i) {
    rgb[i] += m;
    rgb[i] *= 255.f;
  }

  return Color(rgb[0] + 0.5f, rgb[1] + 0.5f, rgb[2] + 0.5f, a * 255.f + 0.5f);
}
}  // namespace cru::platform
