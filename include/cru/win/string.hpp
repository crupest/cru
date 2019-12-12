#pragma once
#include "win_pre_config.hpp"

#include <string>
#include <string_view>

namespace cru::platform::win {
std::string ToUtf8String(const std::wstring_view& string);
std::wstring ToUtf16String(const std::string_view& string);
}
