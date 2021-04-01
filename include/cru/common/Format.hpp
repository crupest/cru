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
  return ToUtf16(std::to_string(number));
}
}  // namespace cru
