#pragma once
#include "Base.hpp"

#include "StringUtil.hpp"

#include <array>
#include <charconv>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>

namespace cru {
template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
std::u16string ToUtf16String(T number) {
  std::array<char, 40> buffer;
  auto result =
      std::to_chars(buffer.data(), buffer.data() + buffer.size(), number);
  Ensures(result.ec == std::errc());
  std::string_view utf8_result(buffer.data(), result.ptr - buffer.data());
  return ToUtf16(utf8_result);
}
}  // namespace cru
