#pragma once
#include "Base.h"

#include "cru/common/Base.h"
#include "cru/common/Format.h"
#include "cru/common/String.h"

#include <cstdint>
#include <optional>
#include <unordered_map>

namespace cru::platform {
struct CRU_PLATFORM_API Color {
  constexpr Color() : Color(0, 0, 0, 255) {}
  constexpr Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue,
                  std::uint8_t alpha = 255)
      : red(red), green(green), blue(blue), alpha(alpha) {}

  constexpr static Color FromHex(std::uint32_t hex) {
    const std::uint32_t mask = 0b11111111;
    return Color((hex >> 16) & mask, (hex >> 8) & mask, hex & mask, 255);
  }

  constexpr static Color FromHexAlpha(std::uint32_t hex) {
    const std::uint32_t mask = 0b11111111;
    return Color((hex >> 24) & mask, (hex >> 16) & mask, (hex >> 8) & mask,
                 hex & mask);
  }

  constexpr Color WithAlpha(std::uint8_t new_alpha) const {
    auto result = *this;
    result.alpha = new_alpha;
    return result;
  }

  float GetFloatRed() const { return static_cast<float>(red) / 255.f; }
  float GetFloatGreen() const { return static_cast<float>(green) / 255.f; }
  float GetFloatBlue() const { return static_cast<float>(blue) / 255.f; }
  float GetFloatAlpha() const { return static_cast<float>(alpha) / 255.f; }

  String ToString() const;

  std::uint8_t red;
  std::uint8_t green;
  std::uint8_t blue;
  std::uint8_t alpha;

  static std::optional<Color> Parse(StringView string,
                                    bool parse_predefined_color = true);
};

constexpr int Compare(const Color& left, const Color& right) {
  if (left.red < right.red) return -1;
  if (left.red > right.red) return 1;
  if (left.green < right.green) return -1;
  if (left.green > right.green) return 1;
  if (left.blue < right.blue) return -1;
  if (left.blue > right.blue) return 1;
  if (left.alpha < right.alpha) return -1;
  if (left.alpha > right.alpha) return 1;
  return 0;
}

constexpr bool operator==(const Color& left, const Color& right) {
  return Compare(left, right) == 0;
}

constexpr bool operator!=(const Color& left, const Color& right) {
  return Compare(left, right) != 0;
}

constexpr bool operator<(const Color& left, const Color& right) {
  return Compare(left, right) < 0;
}

constexpr bool operator<=(const Color& left, const Color& right) {
  return Compare(left, right) <= 0;
}

constexpr bool operator>(const Color& left, const Color& right) {
  return Compare(left, right) > 0;
}

constexpr bool operator>=(const Color& left, const Color& right) {
  return Compare(left, right) >= 0;
}

namespace colors {
constexpr Color transparent = Color::FromHexAlpha(0x00000000);
constexpr Color black = Color::FromHex(0x000000);
constexpr Color silver = Color::FromHex(0xc0c0c0);
constexpr Color gray = Color::FromHex(0x808080);
constexpr Color white = Color::FromHex(0xffffff);
constexpr Color maroon = Color::FromHex(0x800000);
constexpr Color red = Color::FromHex(0xff0000);
constexpr Color purple = Color::FromHex(0x800080);
constexpr Color fuchsia = Color::FromHex(0xff00ff);
constexpr Color green = Color::FromHex(0x008000);
constexpr Color lime = Color::FromHex(0x00ff00);
constexpr Color olive = Color::FromHex(0x808000);
constexpr Color yellow = Color::FromHex(0xffff00);
constexpr Color navy = Color::FromHex(0x000080);
constexpr Color blue = Color::FromHex(0x0000ff);
constexpr Color teal = Color::FromHex(0x008080);
constexpr Color aqua = Color::FromHex(0x00ffff);
constexpr Color orange = Color::FromHex(0xffa500);
constexpr Color aliceblue = Color::FromHex(0xf0f8ff);
constexpr Color antiquewhite = Color::FromHex(0xfaebd7);
constexpr Color aquamarine = Color::FromHex(0x7fffd4);
constexpr Color azure = Color::FromHex(0xf0ffff);
constexpr Color beige = Color::FromHex(0xf5f5dc);
constexpr Color bisque = Color::FromHex(0xffe4c4);
constexpr Color blanchedalmond = Color::FromHex(0xffebcd);
constexpr Color blueviolet = Color::FromHex(0x8a2be2);
constexpr Color brown = Color::FromHex(0xa52a2a);
constexpr Color burlywood = Color::FromHex(0xdeb887);
constexpr Color cadetblue = Color::FromHex(0x5f9ea0);
constexpr Color chartreuse = Color::FromHex(0x7fff00);
constexpr Color chocolate = Color::FromHex(0xd2691e);
constexpr Color coral = Color::FromHex(0xff7f50);
constexpr Color cornflowerblue = Color::FromHex(0x6495ed);
constexpr Color cornsilk = Color::FromHex(0xfff8dc);
constexpr Color crimson = Color::FromHex(0xdc143c);
constexpr Color cyan = aqua;
constexpr Color darkblue = Color::FromHex(0x00008b);
constexpr Color darkcyan = Color::FromHex(0x008b8b);
constexpr Color darkgoldenrod = Color::FromHex(0xb8860b);
constexpr Color darkgray = Color::FromHex(0xa9a9a9);
constexpr Color darkgreen = Color::FromHex(0x006400);
constexpr Color darkgrey = Color::FromHex(0xa9a9a9);
constexpr Color darkkhaki = Color::FromHex(0xbdb76b);
constexpr Color darkmagenta = Color::FromHex(0x8b008b);
constexpr Color darkolivegreen = Color::FromHex(0x556b2f);
constexpr Color darkorange = Color::FromHex(0xff8c00);
constexpr Color darkorchid = Color::FromHex(0x9932cc);
constexpr Color darkred = Color::FromHex(0x8b0000);
constexpr Color darksalmon = Color::FromHex(0xe9967a);
constexpr Color darkseagreen = Color::FromHex(0x8fbc8f);
constexpr Color darkslateblue = Color::FromHex(0x483d8b);
constexpr Color darkslategray = Color::FromHex(0x2f4f4f);
constexpr Color darkslategrey = Color::FromHex(0x2f4f4f);
constexpr Color darkturquoise = Color::FromHex(0x00ced1);
constexpr Color darkviolet = Color::FromHex(0x9400d3);
constexpr Color deeppink = Color::FromHex(0xff1493);
constexpr Color deepskyblue = Color::FromHex(0x00bfff);
constexpr Color dimgray = Color::FromHex(0x696969);
constexpr Color dimgrey = Color::FromHex(0x696969);
constexpr Color dodgerblue = Color::FromHex(0x1e90ff);
constexpr Color firebrick = Color::FromHex(0xb22222);
constexpr Color floralwhite = Color::FromHex(0xfffaf0);
constexpr Color forestgreen = Color::FromHex(0x228b22);
constexpr Color gainsboro = Color::FromHex(0xdcdcdc);
constexpr Color ghostwhite = Color::FromHex(0xf8f8ff);
constexpr Color gold = Color::FromHex(0xffd700);
constexpr Color goldenrod = Color::FromHex(0xdaa520);
constexpr Color greenyellow = Color::FromHex(0xadff2f);
constexpr Color grey = Color::FromHex(0x808080);
constexpr Color honeydew = Color::FromHex(0xf0fff0);
constexpr Color hotpink = Color::FromHex(0xff69b4);
constexpr Color indianred = Color::FromHex(0xcd5c5c);
constexpr Color indigo = Color::FromHex(0x4b0082);
constexpr Color ivory = Color::FromHex(0xfffff0);
constexpr Color khaki = Color::FromHex(0xf0e68c);
constexpr Color lavender = Color::FromHex(0xe6e6fa);
constexpr Color lavenderblush = Color::FromHex(0xfff0f5);
constexpr Color lawngreen = Color::FromHex(0x7cfc00);
constexpr Color lemonchiffon = Color::FromHex(0xfffacd);
constexpr Color lightblue = Color::FromHex(0xadd8e6);
constexpr Color lightcoral = Color::FromHex(0xf08080);
constexpr Color lightcyan = Color::FromHex(0xe0ffff);
constexpr Color lightgoldenrodyellow = Color::FromHex(0xfafad2);
constexpr Color lightgray = Color::FromHex(0xd3d3d3);
constexpr Color lightgreen = Color::FromHex(0x90ee90);
constexpr Color lightgrey = Color::FromHex(0xd3d3d3);
constexpr Color lightpink = Color::FromHex(0xffb6c1);
constexpr Color lightsalmon = Color::FromHex(0xffa07a);
constexpr Color lightseagreen = Color::FromHex(0x20b2aa);
constexpr Color lightskyblue = Color::FromHex(0x87cefa);
constexpr Color lightslategray = Color::FromHex(0x778899);
constexpr Color lightslategrey = Color::FromHex(0x778899);
constexpr Color lightsteelblue = Color::FromHex(0xb0c4de);
constexpr Color lightyellow = Color::FromHex(0xffffe0);
constexpr Color limegreen = Color::FromHex(0x32cd32);
constexpr Color linen = Color::FromHex(0xfaf0e6);
constexpr Color magenta = fuchsia;
constexpr Color mediumaquamarine = Color::FromHex(0x66cdaa);
constexpr Color mediumblue = Color::FromHex(0x0000cd);
constexpr Color mediumorchid = Color::FromHex(0xba55d3);
constexpr Color mediumpurple = Color::FromHex(0x9370db);
constexpr Color mediumseagreen = Color::FromHex(0x3cb371);
constexpr Color mediumslateblue = Color::FromHex(0x7b68ee);
constexpr Color mediumspringgreen = Color::FromHex(0x00fa9a);
constexpr Color mediumturquoise = Color::FromHex(0x48d1cc);
constexpr Color mediumvioletred = Color::FromHex(0xc71585);
constexpr Color midnightblue = Color::FromHex(0x191970);
constexpr Color mintcream = Color::FromHex(0xf5fffa);
constexpr Color mistyrose = Color::FromHex(0xffe4e1);
constexpr Color moccasin = Color::FromHex(0xffe4b5);
constexpr Color navajowhite = Color::FromHex(0xffdead);
constexpr Color oldlace = Color::FromHex(0xfdf5e6);
constexpr Color olivedrab = Color::FromHex(0x6b8e23);
constexpr Color orangered = Color::FromHex(0xff4500);
constexpr Color orchid = Color::FromHex(0xda70d6);
constexpr Color palegoldenrod = Color::FromHex(0xeee8aa);
constexpr Color palegreen = Color::FromHex(0x98fb98);
constexpr Color paleturquoise = Color::FromHex(0xafeeee);
constexpr Color palevioletred = Color::FromHex(0xdb7093);
constexpr Color papayawhip = Color::FromHex(0xffefd5);
constexpr Color peachpuff = Color::FromHex(0xffdab9);
constexpr Color peru = Color::FromHex(0xcd853f);
constexpr Color pink = Color::FromHex(0xffc0cb);
constexpr Color plum = Color::FromHex(0xdda0dd);
constexpr Color powderblue = Color::FromHex(0xb0e0e6);
constexpr Color rosybrown = Color::FromHex(0xbc8f8f);
constexpr Color royalblue = Color::FromHex(0x4169e1);
constexpr Color saddlebrown = Color::FromHex(0x8b4513);
constexpr Color salmon = Color::FromHex(0xfa8072);
constexpr Color sandybrown = Color::FromHex(0xf4a460);
constexpr Color seagreen = Color::FromHex(0x2e8b57);
constexpr Color seashell = Color::FromHex(0xfff5ee);
constexpr Color sienna = Color::FromHex(0xa0522d);
constexpr Color skyblue = Color::FromHex(0x87ceeb);
constexpr Color slateblue = Color::FromHex(0x6a5acd);
constexpr Color slategray = Color::FromHex(0x708090);
constexpr Color slategrey = Color::FromHex(0x708090);
constexpr Color snow = Color::FromHex(0xfffafa);
constexpr Color springgreen = Color::FromHex(0x00ff7f);
constexpr Color steelblue = Color::FromHex(0x4682b4);
constexpr Color tan = Color::FromHex(0xd2b48c);
constexpr Color thistle = Color::FromHex(0xd8bfd8);
constexpr Color tomato = Color::FromHex(0xff6347);
constexpr Color turquoise = Color::FromHex(0x40e0d0);
constexpr Color violet = Color::FromHex(0xee82ee);
constexpr Color wheat = Color::FromHex(0xf5deb3);
constexpr Color whitesmoke = Color::FromHex(0xf5f5f5);
constexpr Color yellowgreen = Color::FromHex(0x9acd32);
constexpr Color rebeccapurple = Color::FromHex(0x663399);
}  // namespace colors
}  // namespace cru::platform

template <>
struct std::hash<cru::platform::Color> {
  std::size_t operator()(const cru::platform::Color& color) const {
    std::size_t seed = 0;
    cru::hash_combine(seed, color.red);
    cru::hash_combine(seed, color.green);
    cru::hash_combine(seed, color.blue);
    cru::hash_combine(seed, color.alpha);
    return seed;
  }
};

namespace cru::platform {
namespace details {
extern const std::unordered_map<StringView, Color> predefined_name_color_map;
}  // namespace details

std::optional<Color> GetPredefinedColorByName(StringView name);

struct CRU_PLATFORM_API HslColor {
  HslColor() = default;
  HslColor(float h, float s, float l, float a = 1.0f)
      : h(h), s(s), l(l), a(a) {}
  HslColor(const Color& rgb);

  operator Color() const;

  float h;
  float s;
  float l;
  float a;
};
}  // namespace cru::platform

namespace cru {
inline String ToString(const platform::Color& color) {
  return cru::Format(u"rgba({}, {}, {}, {})", color.red, color.green,
                     color.blue, color.alpha);
}
}  // namespace cru
