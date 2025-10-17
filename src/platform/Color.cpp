#include "cru/platform/Color.h"

#include <cmath>
#include <cstdint>
#include <optional>

namespace cru::platform {
std::string Color::ToString() const {
  auto to_hex = [](std::uint8_t v) -> char {
    return v >= 10 ? v - 10 + 'a' : v + '0';
  };

  auto to_two_hex_digit = [to_hex](std::uint8_t v) -> std::string {
    return {to_hex(v /= 16), to_hex(v %= 16)};
  };

  std::string result = "#";
  result.append(to_two_hex_digit(red));
  result.append(to_two_hex_digit(green));
  result.append(to_two_hex_digit(blue));
  result.append(to_two_hex_digit(alpha));
  return result;
}

std::optional<Color> Color::Parse(std::string_view string,
                                  bool parse_predefined_color) {
  if (parse_predefined_color) {
    auto optional_predefined_color = GetPredefinedColorByName(string);
    if (optional_predefined_color) {
      return *optional_predefined_color;
    }
  }

  auto get_num = [](char c) -> std::optional<int> {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return std::nullopt;
  };

  auto get_num_for_two_digit =
      [get_num](std::string_view str) -> std::optional<int> {
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
    return Color(static_cast<std::uint8_t>(*r), static_cast<std::uint8_t>(*g),
                 static_cast<std::uint8_t>(*b));
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
    return Color(static_cast<std::uint8_t>(*r), static_cast<std::uint8_t>(*g),
                 static_cast<std::uint8_t>(*b), static_cast<std::uint8_t>(*a));
  } else {
    return std::nullopt;
  }
}

namespace {
const std::unordered_map<std::string_view, Color> predefined_name_color_map{
    {"transparent", colors::transparent},
    {"black", colors::black},
    {"silver", colors::silver},
    {"gray", colors::gray},
    {"white", colors::white},
    {"maroon", colors::maroon},
    {"red", colors::red},
    {"purple", colors::purple},
    {"fuchsia", colors::fuchsia},
    {"green", colors::green},
    {"lime", colors::lime},
    {"olive", colors::olive},
    {"yellow", colors::yellow},
    {"navy", colors::navy},
    {"blue", colors::blue},
    {"teal", colors::teal},
    {"aqua", colors::aqua},
    {"orange", colors::orange},
    {"aliceblue", colors::aliceblue},
    {"antiquewhite", colors::antiquewhite},
    {"aquamarine", colors::aquamarine},
    {"azure", colors::azure},
    {"beige", colors::beige},
    {"bisque", colors::bisque},
    {"blanchedalmond", colors::blanchedalmond},
    {"blueviolet", colors::blueviolet},
    {"brown", colors::brown},
    {"burlywood", colors::burlywood},
    {"cadetblue", colors::cadetblue},
    {"chartreuse", colors::chartreuse},
    {"chocolate", colors::chocolate},
    {"coral", colors::coral},
    {"cornflowerblue", colors::cornflowerblue},
    {"cornsilk", colors::cornsilk},
    {"crimson", colors::crimson},
    {"cyan", colors::cyan},
    {"darkblue", colors::darkblue},
    {"darkcyan", colors::darkcyan},
    {"darkgoldenrod", colors::darkgoldenrod},
    {"darkgray", colors::darkgray},
    {"darkgreen", colors::darkgreen},
    {"darkgrey", colors::darkgrey},
    {"darkkhaki", colors::darkkhaki},
    {"darkmagenta", colors::darkmagenta},
    {"darkolivegreen", colors::darkolivegreen},
    {"darkorange", colors::darkorange},
    {"darkorchid", colors::darkorchid},
    {"darkred", colors::darkred},
    {"darksalmon", colors::darksalmon},
    {"darkseagreen", colors::darkseagreen},
    {"darkslateblue", colors::darkslateblue},
    {"darkslategray", colors::darkslategray},
    {"darkslategrey", colors::darkslategrey},
    {"darkturquoise", colors::darkturquoise},
    {"darkviolet", colors::darkviolet},
    {"deeppink", colors::deeppink},
    {"deepskyblue", colors::deepskyblue},
    {"dimgray", colors::dimgray},
    {"dimgrey", colors::dimgrey},
    {"dodgerblue", colors::dodgerblue},
    {"firebrick", colors::firebrick},
    {"floralwhite", colors::floralwhite},
    {"forestgreen", colors::forestgreen},
    {"gainsboro", colors::gainsboro},
    {"ghostwhite", colors::ghostwhite},
    {"gold", colors::gold},
    {"goldenrod", colors::goldenrod},
    {"greenyellow", colors::greenyellow},
    {"grey", colors::grey},
    {"honeydew", colors::honeydew},
    {"hotpink", colors::hotpink},
    {"indianred", colors::indianred},
    {"indigo", colors::indigo},
    {"ivory", colors::ivory},
    {"khaki", colors::khaki},
    {"lavender", colors::lavender},
    {"lavenderblush", colors::lavenderblush},
    {"lawngreen", colors::lawngreen},
    {"lemonchiffon", colors::lemonchiffon},
    {"lightblue", colors::lightblue},
    {"lightcoral", colors::lightcoral},
    {"lightcyan", colors::lightcyan},
    {"lightgoldenrodyellow", colors::lightgoldenrodyellow},
    {"lightgray", colors::lightgray},
    {"lightgreen", colors::lightgreen},
    {"lightgrey", colors::lightgrey},
    {"lightpink", colors::lightpink},
    {"lightsalmon", colors::lightsalmon},
    {"lightseagreen", colors::lightseagreen},
    {"lightskyblue", colors::lightskyblue},
    {"lightslategray", colors::lightslategray},
    {"lightslategrey", colors::lightslategrey},
    {"lightsteelblue", colors::lightsteelblue},
    {"lightyellow", colors::lightyellow},
    {"limegreen", colors::limegreen},
    {"linen", colors::linen},
    {"magenta", colors::magenta},
    {"mediumaquamarine", colors::mediumaquamarine},
    {"mediumblue", colors::mediumblue},
    {"mediumorchid", colors::mediumorchid},
    {"mediumpurple", colors::mediumpurple},
    {"mediumseagreen", colors::mediumseagreen},
    {"mediumslateblue", colors::mediumslateblue},
    {"mediumspringgreen", colors::mediumspringgreen},
    {"mediumturquoise", colors::mediumturquoise},
    {"mediumvioletred", colors::mediumvioletred},
    {"midnightblue", colors::midnightblue},
    {"mintcream", colors::mintcream},
    {"mistyrose", colors::mistyrose},
    {"moccasin", colors::moccasin},
    {"navajowhite", colors::navajowhite},
    {"oldlace", colors::oldlace},
    {"olivedrab", colors::olivedrab},
    {"orangered", colors::orangered},
    {"orchid", colors::orchid},
    {"palegoldenrod", colors::palegoldenrod},
    {"palegreen", colors::palegreen},
    {"paleturquoise", colors::paleturquoise},
    {"palevioletred", colors::palevioletred},
    {"papayawhip", colors::papayawhip},
    {"peachpuff", colors::peachpuff},
    {"peru", colors::peru},
    {"pink", colors::pink},
    {"plum", colors::plum},
    {"powderblue", colors::powderblue},
    {"rosybrown", colors::rosybrown},
    {"royalblue", colors::royalblue},
    {"saddlebrown", colors::saddlebrown},
    {"salmon", colors::salmon},
    {"sandybrown", colors::sandybrown},
    {"seagreen", colors::seagreen},
    {"seashell", colors::seashell},
    {"sienna", colors::sienna},
    {"skyblue", colors::skyblue},
    {"slateblue", colors::slateblue},
    {"slategray", colors::slategray},
    {"slategrey", colors::slategrey},
    {"snow", colors::snow},
    {"springgreen", colors::springgreen},
    {"steelblue", colors::steelblue},
    {"tan", colors::tan},
    {"thistle", colors::thistle},
    {"tomato", colors::tomato},
    {"turquoise", colors::turquoise},
    {"violet", colors::violet},
    {"wheat", colors::wheat},
    {"whitesmoke", colors::whitesmoke},
    {"yellowgreen", colors::yellowgreen},
    {"rebeccapurple", colors::rebeccapurple},
};
}  // namespace

std::optional<Color> GetPredefinedColorByName(std::string_view name) {
  auto result = predefined_name_color_map.find(name);
  if (result != predefined_name_color_map.cend()) {
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
